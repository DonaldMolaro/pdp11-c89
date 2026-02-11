#include "sema_internal.h"

void sema_fold_constants(Node *node) {
    if (!node) return;

    if (node->kind == ND_BLOCK) {
        Node *n;
        for (n = node->body; n; n = n->next) sema_fold_constants(n);
        return;
    }

    sema_fold_constants(node->lhs);
    sema_fold_constants(node->rhs);
    sema_fold_constants(node->cond);
    sema_fold_constants(node->then);
    sema_fold_constants(node->els);
    sema_fold_constants(node->init);
    sema_fold_constants(node->inc);
    sema_fold_constants(node->body);
    {
        Node *arg;
        for (arg = node->args; arg; arg = arg->next) sema_fold_constants(arg);
    }

    if (node->kind == ND_SIZEOF && node->lhs && node->lhs->ty) {
        node->val = node->lhs->ty->size;
    }
}
