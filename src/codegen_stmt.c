#include "codegen_internal.h"
#include "emitter.h"

#include <stdio.h>

static int next_label(CodegenContext *ctx);

static void emit_br_label(CodegenContext *ctx, const char *label) {
    (void)ctx;
    emit_jmp(label);
}

static void emit_beq_label(CodegenContext *ctx, const char *label) {
    int c = next_label(ctx);
    emitln("    BNE .Lskip%d", c);
    emitln("    JMP %s", label);
    emitln(".Lskip%d:", c);
}

static void emit_br(CodegenContext *ctx, const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_br_label(ctx, buf);
}

static void emit_beq(CodegenContext *ctx, const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_beq_label(ctx, buf);
}

static void emit_br_name(CodegenContext *ctx, const char *fmt, const char *name) {
    char buf[64];
    sprintf(buf, fmt, name);
    emit_br_label(ctx, buf);
}

static int next_label(CodegenContext *ctx) {
    return ctx->labelseq++;
}

static void push_break(CodegenContext *ctx, int label) {
    ctx->break_labels[ctx->break_depth++] = label;
}

static void pop_break(CodegenContext *ctx) {
    if (ctx->break_depth > 0) ctx->break_depth--;
}

static void push_continue(CodegenContext *ctx, int label) {
    ctx->continue_labels[ctx->continue_depth++] = label;
}

static void pop_continue(CodegenContext *ctx) {
    if (ctx->continue_depth > 0) ctx->continue_depth--;
}

static int current_break(CodegenContext *ctx) {
    return ctx->break_depth ? ctx->break_labels[ctx->break_depth - 1] : -1;
}

static int current_continue(CodegenContext *ctx) {
    return ctx->continue_depth ? ctx->continue_labels[ctx->continue_depth - 1] : -1;
}

void gen_stmt(CodegenContext *ctx, Node *node) {
    int c;

    switch (node->kind) {
        case ND_RETURN:
            if (ctx->current_fn_obj && ctx->current_fn_obj->has_sret) {
                if (node->lhs) {
                    int size = ctx->current_fn_obj->ty->return_ty->size;
                    if (node->lhs->kind == ND_FUNCALL &&
                        (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION)) {
                        emitln("    MOV 4(R4), R3");
                        gen_expr(ctx, node->lhs); /* src in R0 */
                        emitln("    MOV R3, R2");
                    } else {
                        emitln("    MOV 4(R4), R2");
                        gen_expr(ctx, node->lhs);
                    }
                    emitln("    MOV #%d, R1", size);
                    emitln("    MOV R1, -(R6)");
                    emitln("    MOV R0, -(R6)");
                    emitln("    MOV R2, -(R6)");
                    emitln("    JSR R5, __memcpy");
                    emitln("    ADD #6, R6");
                    if (node->lhs->kind == ND_FUNCALL &&
                        (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION)) {
                        emitln("    ADD #%d, R6", size);
                    }
                }
                emit_br_name(ctx, ".Lreturn_%s", ctx->current_fn);
                return;
            }
            if (node->lhs) gen_expr(ctx, node->lhs);
            emit_br_name(ctx, ".Lreturn_%s", ctx->current_fn);
            return;
        case ND_IF:
            c = next_label(ctx);
            gen_expr(ctx, node->cond);
            emitln("    TST R0");
            emit_beq(ctx, ".Lelse%d", c);
            gen_stmt(ctx, node->then);
            emit_br(ctx, ".Lend%d", c);
            emitln(".Lelse%d:", c);
            if (node->els) gen_stmt(ctx, node->els);
            emitln(".Lend%d:", c);
            return;
        case ND_WHILE:
            c = next_label(ctx);
            emitln(".Lbegin%d:", c);
            emitln(".Lcontinue%d:", c);
            gen_expr(ctx, node->cond);
            emitln("    TST R0");
            emit_beq(ctx, ".Lend%d", c);
            push_break(ctx, c);
            push_continue(ctx, c);
            gen_stmt(ctx, node->body);
            pop_continue(ctx);
            pop_break(ctx);
            emit_br(ctx, ".Lbegin%d", c);
            emitln(".Lend%d:", c);
            return;
        case ND_FOR: {
            int begin = next_label(ctx);
            int end = next_label(ctx);
            int cont = next_label(ctx);
            if (node->init) {
                if (node->init->kind == ND_BLOCK || node->init->kind == ND_EXPR_STMT || node->init->kind == ND_NULL)
                    gen_stmt(ctx, node->init);
                else
                    gen_expr(ctx, node->init);
            }
            emitln(".Lbegin%d:", begin);
            if (node->cond) {
                gen_expr(ctx, node->cond);
                emitln("    TST R0");
                emit_beq(ctx, ".Lend%d", end);
            }
            push_break(ctx, end);
            push_continue(ctx, cont);
            gen_stmt(ctx, node->body);
            emitln(".Lcontinue%d:", cont);
            if (node->inc) gen_expr(ctx, node->inc);
            pop_continue(ctx);
            pop_break(ctx);
            emit_br(ctx, ".Lbegin%d", begin);
            emitln(".Lend%d:", end);
            return;
        }
        case ND_BREAK:
            if (current_break(ctx) >= 0)
                emit_br(ctx, ".Lend%d", current_break(ctx));
            return;
        case ND_CONTINUE:
            if (current_continue(ctx) >= 0)
                emit_br(ctx, ".Lcontinue%d", current_continue(ctx));
            return;
        case ND_SWITCH: {
            int end = next_label(ctx);
            Node *cnode;
            gen_expr(ctx, node->cond);
            for (cnode = node->case_next; cnode; cnode = cnode->case_next) {
                cnode->case_label = next_label(ctx);
                emitln("    CMP #%ld, R0", cnode->val);
                emit_beq(ctx, ".Lcase%d", cnode->case_label);
            }
            if (node->default_case) {
                node->default_case->case_label = next_label(ctx);
                emit_br(ctx, ".Lcase%d", node->default_case->case_label);
            } else {
                emit_br(ctx, ".Lend%d", end);
            }
            push_break(ctx, end);
            gen_stmt(ctx, node->body);
            pop_break(ctx);
            emitln(".Lend%d:", end);
            return;
        }
        case ND_CASE:
            emitln(".Lcase%d:", node->case_label);
            if (node->body) gen_stmt(ctx, node->body);
            return;
        case ND_BLOCK: {
            Node *stmt;
            for (stmt = node->body; stmt; stmt = stmt->next) {
                gen_stmt(ctx, stmt);
            }
            return;
        }
        case ND_EXPR_STMT:
            if (node->lhs) gen_expr(ctx, node->lhs);
            return;
        case ND_NULL:
            return;
    }
}
