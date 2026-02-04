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

Token *tokenize(char *input) {
    Token head;
    Token *cur = &head;
    char *p = input;

    set_current_input(input);

    while (*p) {
        if (isspace((unsigned char)*p)) { p++; continue; }

        if (startswith(p, "//")) {
            p += 2;
            while (*p && *p != '\n') p++;
            continue;
        }
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) error_at(p, "unterminated comment");
            p = q + 2;
            continue;
        }

        if (*p == '"') {
            char *start = p++;
            char *buf = xcalloc(1, 4096);
            int len = 0;
            while (*p && *p != '"') {
                if (*p == '\\') {
                    p++;
                    buf[len++] = (char)read_escape(&p);
                    continue;
                }
                buf[len++] = *p++;
            }
            if (*p != '"') error_at(start, "unterminated string");
            p++;
            Token *tok = new_token(TK_STR, start, p);
            tok->str = buf;
            tok->str_len = len;
            cur = cur->next = tok;
            continue;
        }

        if (*p == '\'') {
            char *start = p++;
            int c;
            if (*p == '\\') {
                p++;
                c = read_escape(&p);
            } else {
                c = *p++;
            }
            if (*p != '\'') error_at(start, "unterminated char literal");
            p++;
            Token *tok = new_token(TK_CHAR, start, p);
            tok->val = c;
            cur = cur->next = tok;
            continue;
        }

        if (isdigit((unsigned char)*p)) {
            char *start = p;
            long val = read_number(&p);
            Token *tok = new_token(TK_NUM, start, p);
            tok->val = val;
            cur = cur->next = tok;
            continue;
        }

        if (is_ident1(*p)) {
            char *start = p;
            p++;
            while (is_ident2(*p)) p++;
            Token *tok = new_token(TK_IDENT, start, p);
            tok->str = strndup2(start, tok->len);

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

            cur = cur->next = tok;
            continue;
        }

        if (startswith(p, "...")) { cur = cur->next = new_token(TK_ELLIPSIS, p, p + 3); p += 3; continue; }
        if (startswith(p, "++")) { cur = cur->next = new_token(TK_INC, p, p + 2); p += 2; continue; }
        if (startswith(p, "+=")) { cur = cur->next = new_token(TK_ADD_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "--")) { cur = cur->next = new_token(TK_DEC, p, p + 2); p += 2; continue; }
        if (startswith(p, "-=")) { cur = cur->next = new_token(TK_SUB_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "->")) { cur = cur->next = new_token(TK_ARROW, p, p + 2); p += 2; continue; }
        if (startswith(p, "==")) { cur = cur->next = new_token(TK_EQ, p, p + 2); p += 2; continue; }
        if (startswith(p, "!=")) { cur = cur->next = new_token(TK_NE, p, p + 2); p += 2; continue; }
        if (startswith(p, "<=")) { cur = cur->next = new_token(TK_LE, p, p + 2); p += 2; continue; }
        if (startswith(p, ">=")) { cur = cur->next = new_token(TK_GE, p, p + 2); p += 2; continue; }
        if (startswith(p, "&&")) { cur = cur->next = new_token(TK_AND, p, p + 2); p += 2; continue; }
        if (startswith(p, "||")) { cur = cur->next = new_token(TK_OR, p, p + 2); p += 2; continue; }
        if (startswith(p, "<<")) { cur = cur->next = new_token(TK_SHL, p, p + 2); p += 2; continue; }
        if (startswith(p, ">>")) { cur = cur->next = new_token(TK_SHR, p, p + 2); p += 2; continue; }

        if (strchr("+-*/%(){}[];,<>=!&|.^~:?", *p)) {
            cur = cur->next = new_token(*p, p, p + 1);
            p++;
            continue;
        }

        error_at(p, "invalid token");
    }

    cur->next = new_token(TK_EOF, p, p);
    return head.next;
}
