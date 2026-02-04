#include "compiler.h"

#include <string.h>

static Type ty_int_obj;
static Type ty_char_obj;
static Type ty_void_obj;

Type *ty_int(void) {
    ty_int_obj.kind = TY_INT;
    ty_int_obj.size = 2;
    ty_int_obj.align = 2;
    return &ty_int_obj;
}

Type *ty_char(void) {
    ty_char_obj.kind = TY_CHAR;
    ty_char_obj.size = 1;
    ty_char_obj.align = 1;
    return &ty_char_obj;
}

Type *ty_void(void) {
    ty_void_obj.kind = TY_VOID;
    ty_void_obj.size = 1;
    ty_void_obj.align = 1;
    return &ty_void_obj;
}

Type *pointer_to(Type *base) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_PTR;
    ty->base = base;
    ty->size = 2;
    ty->align = 2;
    return ty;
}

Type *array_of(Type *base, int len) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_ARRAY;
    ty->base = base;
    ty->array_len = len;
    ty->size = base->size * len;
    ty->align = base->align;
    return ty;
}

Type *func_type(Type *ret, Type *params) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = TY_FUNC;
    ty->return_ty = ret;
    ty->params = params;
    ty->is_func = 1;
    ty->size = 2;
    ty->align = 2;
    return ty;
}

int is_integer(Type *ty) {
    return ty->kind == TY_INT || ty->kind == TY_CHAR;
}

static int align_to(int n, int align) {
    return (n + align - 1) / align * align;
}

Type *struct_type(int is_union, char *tag, Member *members) {
    Type *ty = xcalloc(1, sizeof(Type));
    ty->kind = is_union ? TY_UNION : TY_STRUCT;
    ty->tag = tag;
    ty->members = members;

    if (!members) {
        ty->is_incomplete = 1;
        ty->size = 0;
        ty->align = 1;
        return ty;
    }

    if (is_union) {
        int max = 0;
        int align = 1;
        Member *m;
        for (m = members; m; m = m->next) {
            if (m->ty->size > max) max = m->ty->size;
            if (m->ty->align > align) align = m->ty->align;
            m->offset = 0;
        }
        ty->align = align;
        ty->size = align_to(max, align);
        return ty;
    }

    {
        int offset = 0;
        int align = 1;
        Member *m;
        for (m = members; m; m = m->next) {
            offset = align_to(offset, m->ty->align);
            m->offset = offset;
            offset += m->ty->size;
            if (m->ty->align > align) align = m->ty->align;
        }
        ty->align = align;
        ty->size = align_to(offset, align);
    }
    return ty;
}

Member *find_member(Type *ty, const char *name) {
    Member *m;
    for (m = ty->members; m; m = m->next) {
        if (strcmp(m->name, name) == 0) return m;
    }
    return NULL;
}
