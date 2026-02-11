#ifndef PDP11_C89_PARSER_SCOPE_H
#define PDP11_C89_PARSER_SCOPE_H

#include "compiler.h"

typedef struct Scope Scope;

Scope *scope_create(void);
Scope *scope_enter(Scope *scope);
Scope *scope_leave(Scope *scope);

void scope_push_var(Scope *scope, char *name, Obj *var);
void scope_push_tag(Scope *scope, char *name, Type *ty);
void scope_push_typedef(Scope *scope, char *name, Type *ty);

Obj *scope_find_var(Scope *scope, Token *tok);
Type *scope_find_tag(Scope *scope, Token *tok);
Type *scope_find_typedef(Scope *scope, Token *tok);

#endif
