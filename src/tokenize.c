#include "compiler.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static Token *new_token(int kind, char *start, char *end) {
    Token *tok = xcalloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = (int)(end - start);
    return tok;
}

static int is_ident1(int c) {
    return isalpha(c) || c == '_';
}

static int is_ident2(int c) {
    return isalnum(c) || c == '_';
}

static long read_number(char **p) {
    char *s = *p;
    int base = 10;
    long val = 0;

    if (s[0] == '0') {
        if (s[1] == 'x' || s[1] == 'X') {
            base = 16;
            s += 2;
        } else if (isdigit((unsigned char)s[1])) {
            base = 8;
            s += 1;
        }
    }

    while (*s) {
        int d;
        if (isdigit((unsigned char)*s)) d = *s - '0';
        else if (*s >= 'a' && *s <= 'f') d = 10 + (*s - 'a');
        else if (*s >= 'A' && *s <= 'F') d = 10 + (*s - 'A');
        else break;
        if (d >= base) break;
        val = val * base + d;
        s++;
    }

    *p = s;
    return val;
}

static int read_escape(char **p) {
    char *s = *p;
    if (*s == 'n') { *p = s + 1; return '\n'; }
    if (*s == 't') { *p = s + 1; return '\t'; }
    if (*s == 'r') { *p = s + 1; return '\r'; }
    if (*s == '\\') { *p = s + 1; return '\\'; }
    if (*s == '\'') { *p = s + 1; return '\''; }
    if (*s == '\"') { *p = s + 1; return '\"'; }
    *p = s + 1;
    return *s;
}

static int skip_space_and_comments(char **p) {
    char *s = *p;
    int did = 0;

    for (;;) {
        if (isspace((unsigned char)*s)) {
            s++;
            did = 1;
            continue;
        }
        if (startswith(s, "//")) {
            s += 2;
            while (*s && *s != '\n') s++;
            did = 1;
            continue;
        }
        if (startswith(s, "/*")) {
            char *q = strstr(s + 2, "*/");
            if (!q) error_at(s, "unterminated comment");
            s = q + 2;
            did = 1;
            continue;
        }
        break;
    }

    *p = s;
    return did;
}

static void classify_keyword(Token *tok) {
    if (tok->len == 6 && strncmp(tok->str, "return", 6) == 0) tok->kind = TK_RETURN;
    else if (tok->len == 2 && strncmp(tok->str, "if", 2) == 0) tok->kind = TK_IF;
    else if (tok->len == 4 && strncmp(tok->str, "else", 4) == 0) tok->kind = TK_ELSE;
    else if (tok->len == 5 && strncmp(tok->str, "while", 5) == 0) tok->kind = TK_WHILE;
    else if (tok->len == 3 && strncmp(tok->str, "for", 3) == 0) tok->kind = TK_FOR;
    else if (tok->len == 5 && strncmp(tok->str, "break", 5) == 0) tok->kind = TK_BREAK;
    else if (tok->len == 8 && strncmp(tok->str, "continue", 8) == 0) tok->kind = TK_CONTINUE;
    else if (tok->len == 6 && strncmp(tok->str, "switch", 6) == 0) tok->kind = TK_SWITCH;
    else if (tok->len == 4 && strncmp(tok->str, "case", 4) == 0) tok->kind = TK_CASE;
    else if (tok->len == 7 && strncmp(tok->str, "default", 7) == 0) tok->kind = TK_DEFAULT;
    else if (tok->len == 3 && strncmp(tok->str, "int", 3) == 0) tok->kind = TK_INT;
    else if (tok->len == 4 && strncmp(tok->str, "char", 4) == 0) tok->kind = TK_CHAR_KW;
    else if (tok->len == 4 && strncmp(tok->str, "void", 4) == 0) tok->kind = TK_VOID;
    else if (tok->len == 6 && strncmp(tok->str, "struct", 6) == 0) tok->kind = TK_STRUCT;
    else if (tok->len == 5 && strncmp(tok->str, "union", 5) == 0) tok->kind = TK_UNION;
    else if (tok->len == 4 && strncmp(tok->str, "enum", 4) == 0) tok->kind = TK_ENUM;
    else if (tok->len == 6 && strncmp(tok->str, "sizeof", 6) == 0) tok->kind = TK_SIZEOF;
    else if (tok->len == 7 && strncmp(tok->str, "typedef", 7) == 0) tok->kind = TK_TYPEDEF;
    else if (tok->len == 6 && strncmp(tok->str, "static", 6) == 0) tok->kind = TK_STATIC;
    else if (tok->len == 6 && strncmp(tok->str, "extern", 6) == 0) tok->kind = TK_EXTERN;
    else if (tok->len == 5 && strncmp(tok->str, "const", 5) == 0) tok->kind = TK_CONST;
    else if (tok->len == 8 && strncmp(tok->str, "volatile", 8) == 0) tok->kind = TK_VOLATILE;
    else if (tok->len == 6 && strncmp(tok->str, "signed", 6) == 0) tok->kind = TK_SIGNED;
    else if (tok->len == 8 && strncmp(tok->str, "unsigned", 8) == 0) tok->kind = TK_UNSIGNED;
    else if (tok->len == 5 && strncmp(tok->str, "short", 5) == 0) tok->kind = TK_SHORT;
    else if (tok->len == 4 && strncmp(tok->str, "long", 4) == 0) tok->kind = TK_LONG;
}

static Token *read_string_literal(char **p) {
    char *start = (*p)++;
    char *buf = xcalloc(1, 4096);
    int len = 0;
    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            buf[len++] = (char)read_escape(p);
            continue;
        }
        buf[len++] = *(*p)++;
    }
    if (**p != '"') error_at(start, "unterminated string");
    (*p)++;
    {
        Token *tok = new_token(TK_STR, start, *p);
        tok->str = buf;
        tok->str_len = len;
        return tok;
    }
}

static Token *read_char_literal(char **p) {
    char *start = (*p)++;
    int c;
    if (**p == '\\') {
        (*p)++;
        c = read_escape(p);
    } else {
        c = *(*p)++;
    }
    if (**p != '\'') error_at(start, "unterminated char literal");
    (*p)++;
    {
        Token *tok = new_token(TK_CHAR, start, *p);
        tok->val = c;
        return tok;
    }
}

static Token *read_number_token(char **p) {
    char *start = *p;
    long val = read_number(p);
    {
        Token *tok = new_token(TK_NUM, start, *p);
        tok->val = val;
        return tok;
    }
}

static Token *read_ident_or_keyword(char **p) {
    char *start = *p;
    (*p)++;
    while (is_ident2(**p)) (*p)++;
    {
        Token *tok = new_token(TK_IDENT, start, *p);
        tok->str = strndup2(start, tok->len);
        classify_keyword(tok);
        return tok;
    }
}

static Token *read_punct(char **p) {
    char *s = *p;
    if (startswith(s, "...")) { *p = s + 3; return new_token(TK_ELLIPSIS, s, s + 3); }
    if (startswith(s, "++")) { *p = s + 2; return new_token(TK_INC, s, s + 2); }
    if (startswith(s, "+=")) { *p = s + 2; return new_token(TK_ADD_EQ, s, s + 2); }
    if (startswith(s, "--")) { *p = s + 2; return new_token(TK_DEC, s, s + 2); }
    if (startswith(s, "-=")) { *p = s + 2; return new_token(TK_SUB_EQ, s, s + 2); }
    if (startswith(s, "->")) { *p = s + 2; return new_token(TK_ARROW, s, s + 2); }
    if (startswith(s, "==")) { *p = s + 2; return new_token(TK_EQ, s, s + 2); }
    if (startswith(s, "!=")) { *p = s + 2; return new_token(TK_NE, s, s + 2); }
    if (startswith(s, "<=")) { *p = s + 2; return new_token(TK_LE, s, s + 2); }
    if (startswith(s, ">=")) { *p = s + 2; return new_token(TK_GE, s, s + 2); }
    if (startswith(s, "&&")) { *p = s + 2; return new_token(TK_AND, s, s + 2); }
    if (startswith(s, "||")) { *p = s + 2; return new_token(TK_OR, s, s + 2); }
    if (startswith(s, "<<")) { *p = s + 2; return new_token(TK_SHL, s, s + 2); }
    if (startswith(s, ">>")) { *p = s + 2; return new_token(TK_SHR, s, s + 2); }
    if (strchr("+-*/%(){}[];,<>=!&|.^~:?", *s)) {
        *p = s + 1;
        return new_token(*s, s, s + 1);
    }
    return NULL;
}

Token *tokenize(char *input) {
    Token head;
    Token *cur = &head;
    char *p = input;

    set_current_input(input);

    while (*p) {
        Token *tok;

        if (skip_space_and_comments(&p)) continue;

        if (*p == '"') {
            cur = cur->next = read_string_literal(&p);
            continue;
        }

        if (*p == '\'') {
            cur = cur->next = read_char_literal(&p);
            continue;
        }

        if (isdigit((unsigned char)*p)) {
            cur = cur->next = read_number_token(&p);
            continue;
        }

        if (is_ident1(*p)) {
            cur = cur->next = read_ident_or_keyword(&p);
            continue;
        }

        tok = read_punct(&p);
        if (tok) {
            cur = cur->next = tok;
            continue;
        }

        error_at(p, "invalid token");
    }

    cur->next = new_token(TK_EOF, p, p);
    return head.next;
}
