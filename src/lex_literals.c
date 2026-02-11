#include "lex_literals.h"

#include <ctype.h>
#include <stdlib.h>

Token *lex_new_token(int kind, char *start, char *end) {
    Token *tok = xcalloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = (int)(end - start);
    return tok;
}

int lex_read_escape(char **p) {
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

long lex_read_number(char **p) {
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

Token *lex_read_string_literal(char **p) {
    char *start = (*p)++;
    char *buf = xcalloc(1, 4096);
    int len = 0;
    while (**p && **p != '"') {
        if (**p == '\\') {
            (*p)++;
            buf[len++] = (char)lex_read_escape(p);
            continue;
        }
        buf[len++] = *(*p)++;
    }
    if (**p != '"') error_at(start, "unterminated string");
    (*p)++;
    {
        Token *tok = lex_new_token(TK_STR, start, *p);
        tok->str = buf;
        tok->str_len = len;
        return tok;
    }
}

Token *lex_read_char_literal(char **p) {
    char *start = (*p)++;
    int c;
    if (**p == '\\') {
        (*p)++;
        c = lex_read_escape(p);
    } else {
        c = *(*p)++;
    }
    if (**p != '\'') error_at(start, "unterminated char literal");
    (*p)++;
    {
        Token *tok = lex_new_token(TK_CHAR, start, *p);
        tok->val = c;
        return tok;
    }
}

Token *lex_read_number_token(char **p) {
    char *start = *p;
    long val = lex_read_number(p);
    {
        Token *tok = lex_new_token(TK_NUM, start, *p);
        tok->val = val;
        return tok;
    }
}
