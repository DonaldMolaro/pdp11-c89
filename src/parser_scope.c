#include "parser_scope.h"

#include <string.h>

typedef struct VarScope VarScope;
struct VarScope {
    VarScope *next;
    char *name;
    Obj *var;
};

typedef struct TagScope TagScope;
struct TagScope {
    TagScope *next;
    char *name;
    Type *ty;
};

typedef struct TypedefScope TypedefScope;
struct TypedefScope {
    TypedefScope *next;
    char *name;
    Type *ty;
};

struct Scope {
    struct Scope *next;
    VarScope *vars;
    TagScope *tags;
    TypedefScope *typedefs;
};

Scope *scope_create(void) {
    return NULL;
}

Scope *scope_enter(Scope *scope) {
    Scope *sc = xcalloc(1, sizeof(Scope));
    sc->next = scope;
    return sc;
}

Scope *scope_leave(Scope *scope) {
    if (!scope) return NULL;
    return scope->next;
}

void scope_push_var(Scope *scope, char *name, Obj *var) {
    VarScope *sc = xcalloc(1, sizeof(VarScope));
    sc->name = name;
    sc->var = var;
    sc->next = scope->vars;
    scope->vars = sc;
}

void scope_push_tag(Scope *scope, char *name, Type *ty) {
    TagScope *sc = xcalloc(1, sizeof(TagScope));
    sc->name = name;
    sc->ty = ty;
    sc->next = scope->tags;
    scope->tags = sc;
}

void scope_push_typedef(Scope *scope, char *name, Type *ty) {
    TypedefScope *sc = xcalloc(1, sizeof(TypedefScope));
    sc->name = name;
    sc->ty = ty;
    sc->next = scope->typedefs;
    scope->typedefs = sc;
}

Obj *scope_find_var(Scope *scope, Token *tok) {
    Scope *sc;
    VarScope *var;

    for (sc = scope; sc; sc = sc->next) {
        for (var = sc->vars; var; var = var->next) {
            if (strlen(var->name) == (size_t)tok->len &&
                strncmp(var->name, tok->loc, tok->len) == 0) {
                return var->var;
            }
        }
    }
    return NULL;
}

Type *scope_find_tag(Scope *scope, Token *tok) {
    Scope *sc;
    TagScope *tag;

    for (sc = scope; sc; sc = sc->next) {
        for (tag = sc->tags; tag; tag = tag->next) {
            if (strlen(tag->name) == (size_t)tok->len &&
                strncmp(tag->name, tok->loc, tok->len) == 0) {
                return tag->ty;
            }
        }
    }
    return NULL;
}

Type *scope_find_typedef(Scope *scope, Token *tok) {
    Scope *sc;
    TypedefScope *td;

    for (sc = scope; sc; sc = sc->next) {
        for (td = sc->typedefs; td; td = td->next) {
            if (strlen(td->name) == (size_t)tok->len &&
                strncmp(td->name, tok->loc, tok->len) == 0) {
                return td->ty;
            }
        }
    }
    return NULL;
}
