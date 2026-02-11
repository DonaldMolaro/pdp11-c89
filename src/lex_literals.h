#ifndef PDP11_C89_LEX_LITERALS_H
#define PDP11_C89_LEX_LITERALS_H

#include "compiler.h"

Token *lex_new_token(int kind, char *start, char *end);
int lex_read_escape(char **p);
long lex_read_number(char **p);
Token *lex_read_string_literal(char **p);
Token *lex_read_char_literal(char **p);
Token *lex_read_number_token(char **p);

#endif
