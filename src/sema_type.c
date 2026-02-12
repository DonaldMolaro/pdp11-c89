#include "sema_internal.h"

#include <stdio.h>
#include <stdlib.h>

void sema_add_type(Node *node) {
    if (!node || node->ty) return;

    if (node->kind == ND_BLOCK) {
        Node *n;
        for (n = node->body; n; n = n->next) sema_add_type(n);
        node->ty = ty_void();
        return;
    }

    sema_add_type(node->lhs);
    sema_add_type(node->rhs);
    sema_add_type(node->cond);
    sema_add_type(node->then);
    sema_add_type(node->els);
    sema_add_type(node->init);
    sema_add_type(node->inc);
    sema_add_type(node->body);
    {
        Node *arg;
        for (arg = node->args; arg; arg = arg->next) sema_add_type(arg);
    }

    switch (node->kind) {
        case ND_NUM:
            node->ty = ty_int();
            return;
        case ND_VAR:
            node->ty = node->var->ty;
            return;
        case ND_ADD:
        case ND_SUB:
            if (node->lhs->ty->kind == TY_PTR || node->lhs->ty->kind == TY_ARRAY) {
                node->ty = node->lhs->ty->kind == TY_ARRAY ? pointer_to(node->lhs->ty->base) : node->lhs->ty;
                return;
            }
            node->ty = ty_int();
            return;
        case ND_MUL:
        case ND_DIV:
        case ND_MOD:
        case ND_BITAND:
        case ND_BITOR:
        case ND_BITXOR:
        case ND_SHL:
        case ND_SHR:
            node->ty = ty_int();
            return;
        case ND_NEG:
        case ND_PRE_INC:
        case ND_PRE_DEC:
        case ND_POST_INC:
        case ND_POST_DEC:
            node->ty = node->lhs->ty;
            return;
        case ND_CAST:
            node->ty = node->cast_ty ? node->cast_ty : node->lhs->ty;
            return;
        case ND_ASSIGN:
        case ND_ADD_EQ:
        case ND_SUB_EQ:
            node->ty = node->lhs->ty;
            return;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE:
        case ND_LOGAND:
        case ND_LOGOR:
        case ND_NOT:
        case ND_BITNOT:
        case ND_SIZEOF:
        case ND_TERNARY:
            node->ty = ty_int();
            return;
        case ND_ADDR:
            if (node->lhs->ty->kind == TY_ARRAY) {
                node->ty = pointer_to(node->lhs->ty->base);
            } else {
                node->ty = pointer_to(node->lhs->ty);
            }
            return;
        case ND_DEREF:
            if (node->lhs->ty->kind == TY_PTR)
                node->ty = node->lhs->ty->base;
            else if (node->lhs->ty->kind == TY_ARRAY)
                node->ty = node->lhs->ty->base;
            else
                node->ty = ty_int();
            return;
        case ND_MEMBER: {
            Type *ty = node->lhs->ty;
            if (ty->kind == TY_PTR) ty = ty->base;
            if (ty->kind != TY_STRUCT && ty->kind != TY_UNION) {
                fprintf(stderr, "not a struct/union\n");
                exit(1);
            }
            node->member = find_member(ty, node->member_name);
            if (!node->member) { fprintf(stderr, "no such member: %s\n", node->member_name); exit(1); }
            node->ty = node->member->ty;
            return;
        }
        case ND_FUNCALL:
            if (node->lhs && node->lhs->ty) {
                Type *fty = node->lhs->ty;
                if (fty->kind == TY_PTR) fty = fty->base;
                if (!fty || fty->kind != TY_FUNC) {
                    fprintf(stderr, "invalid function call\n");
                    exit(1);
                }
                node->ty = fty->return_ty;
                return;
            }
            if (node->var && node->var->ty && node->var->ty->kind == TY_FUNC) {
                node->ty = node->var->ty->return_ty;
            } else {
                fprintf(stderr, "invalid function call\n");
                exit(1);
            }
            return;
        case ND_RETURN:
            node->ty = node->lhs ? node->lhs->ty : ty_void();
            return;
        case ND_EXPR_STMT:
            node->ty = node->lhs ? node->lhs->ty : ty_void();
            return;
        case ND_BLOCK:
        case ND_IF:
        case ND_WHILE:
        case ND_FOR:
        case ND_SWITCH:
        case ND_CASE:
        case ND_BREAK:
        case ND_CONTINUE:
        case ND_NULL:
            node->ty = ty_void();
            return;
    }
}
