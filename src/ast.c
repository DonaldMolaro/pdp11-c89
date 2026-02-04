#include "compiler.h"

#include <string.h>

Node *new_node(int kind) {
    Node *node = xcalloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(int kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_unary(int kind, Node *expr) {
    Node *node = new_node(kind);
    node->lhs = expr;
    return node;
}

Node *new_num(long val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    node->ty = ty_int();
    return node;
}

Node *new_var_node(Obj *var) {
    Node *node = new_node(ND_VAR);
    node->var = var;
    node->ty = var->ty;
    return node;
}

Node *new_block(Node *stmts) {
    Node *node = new_node(ND_BLOCK);
    node->body = stmts;
    return node;
}

Node *new_expr_stmt(Node *expr) {
    Node *node = new_node(ND_EXPR_STMT);
    node->lhs = expr;
    return node;
}
