#include "codegen_internal.h"
#include "emitter.h"

#include <stdio.h>
#include <stdlib.h>

void gen_addr(CodegenContext *ctx, Node *node) {
    if (node->kind == ND_VAR) {
        if (node->var->is_local) {
            emitln("    MOV R4, R0");
            if (node->var->offset != 0)
                emitln("    ADD #%d, R0", node->var->offset);
            return;
        }
        emitln("    MOV #%s, R0", node->var->asm_name);
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_expr(ctx, node->lhs);
        return;
    }
    if (node->kind == ND_MEMBER) {
        gen_addr(ctx, node->lhs);
        if (node->member)
            emitln("    ADD #%d, R0", node->member->offset);
        return;
    }
    fprintf(stderr, "not an lvalue\n");
    exit(1);
}
