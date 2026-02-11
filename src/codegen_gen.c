#include "codegen_internal.h"
#include "emitter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void emit_bne_label(CodegenContext *ctx, const char *label) {
    int c = next_label(ctx);
    emitln("    BEQ .Lskip%d", c);
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

static void emit_bne(CodegenContext *ctx, const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_bne_label(ctx, buf);
}

static void emit_br_name(CodegenContext *ctx, const char *fmt, const char *name) {
    char buf[64];
    sprintf(buf, fmt, name);
    emit_br_label(ctx, buf);
}

static int next_label(CodegenContext *ctx) {
    return ctx->labelseq++;
}

static void gen_expr(CodegenContext *ctx, Node *node);
void gen_stmt(CodegenContext *ctx, Node *node);
static void gen_addr(CodegenContext *ctx, Node *node);

static int inc_amount(Type *ty) {
    if (ty->kind == TY_PTR) return ty->base->size;
    if (ty->kind == TY_ARRAY) return ty->base->size;
    return 1;
}

static void push_r0(void) {
    emitln("    MOV R0, -(R6)");
}

static void pop_r1(void) {
    emitln("    MOV (R6)+, R1");
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

static void gen_addr(CodegenContext *ctx, Node *node) {
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

static void load(Type *ty) {
    if (ty->size == 1) {
        emitln("    MOVB (R0), R0");
    } else {
        emitln("    MOV (R0), R0");
    }
}

static void store(Type *ty) {
    if (ty->size == 1) {
        emitln("    MOVB R0, (R1)");
    } else {
        emitln("    MOV R0, (R1)");
    }
}

static void gen_cmp(CodegenContext *ctx, Node *node, int kind) {
    int c = next_label(ctx);

    gen_expr(ctx, node->lhs);
    push_r0();
    gen_expr(ctx, node->rhs);
    pop_r1();

    emitln("    CMP R0, R1");
    if (kind == ND_EQ)
        emit_beq(ctx, ".Ltrue%d", c);
    else
        emit_bne(ctx, ".Ltrue%d", c);

    emitln("    MOV #0, R0");
    emit_br(ctx, ".Lend%d", c);
    emitln(".Ltrue%d:", c);
    emitln("    MOV #1, R0");
    emitln(".Lend%d:", c);
}

static void gen_rel(CodegenContext *ctx, Node *node, int kind) {
    int c = next_label(ctx);

    gen_expr(ctx, node->lhs);
    push_r0();
    gen_expr(ctx, node->rhs);
    pop_r1();

    emitln("    SUB R0, R1");

    emitln("    MOV R1, R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    ASR R0");
    emitln("    BIC #0xFFFE, R0");

    if (kind == ND_LT) {
        return;
    }

    if (kind == ND_GE) {
        emitln("    MOV #1, R1");
        emitln("    SUB R0, R1");
        emitln("    MOV R1, R0");
        return;
    }

    emitln("    CMP #0, R1");
    emit_beq(ctx, ".Lzero%d", c);
    emitln("    MOV #0, R1");
    emit_br(ctx, ".Lzero_end%d", c);
    emitln(".Lzero%d:", c);
    emitln("    MOV #1, R1");
    emitln(".Lzero_end%d:", c);

    if (kind == ND_LE) {
        emitln("    BIS R1, R0");
        return;
    }

    if (kind == ND_GT) {
        emitln("    BIS R1, R0");
        emitln("    MOV #1, R1");
        emitln("    SUB R0, R1");
        emitln("    MOV R1, R0");
        return;
    }
}

static void gen_expr(CodegenContext *ctx, Node *node) {
    int c;

    switch (node->kind) {
        case ND_NUM:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_VAR:
            gen_addr(ctx, node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_MEMBER:
            gen_addr(ctx, node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_ADDR:
            gen_addr(ctx, node->lhs);
            return;
        case ND_DEREF:
            gen_expr(ctx, node->lhs);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_PRE_INC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(ctx, node->lhs);
            push_r0();
            load(node->lhs->ty);
            if (inc != 0)
                emitln("    ADD #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            return;
        }
        case ND_PRE_DEC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(ctx, node->lhs);
            push_r0();
            load(node->lhs->ty);
            if (inc != 0)
                emitln("    SUB #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            return;
        }
        case ND_POST_INC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(ctx, node->lhs);
            push_r0();
            load(node->lhs->ty);
            emitln("    MOV R0, R2");
            if (inc != 0)
                emitln("    ADD #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            emitln("    MOV R2, R0");
            return;
        }
        case ND_POST_DEC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(ctx, node->lhs);
            push_r0();
            load(node->lhs->ty);
            emitln("    MOV R0, R2");
            if (inc != 0)
                emitln("    SUB #%d, R0", inc);
            pop_r1();
            store(node->lhs->ty);
            emitln("    MOV R2, R0");
            return;
        }
        case ND_ADD_EQ:
        case ND_SUB_EQ: {
            Type *lt = node->lhs->ty;
            int scale = 1;
            int lptr = (lt->kind == TY_PTR || lt->kind == TY_ARRAY);
            if (lptr) scale = lt->base->size;

            gen_addr(ctx, node->lhs);
            push_r0();
            load(lt);
            push_r0();
            gen_expr(ctx, node->rhs);
            if (lptr && scale == 2) emitln("    ADD R0, R0");
            pop_r1();
            if (node->kind == ND_ADD_EQ)
                emitln("    ADD R0, R1");
            else
                emitln("    SUB R0, R1");
            emitln("    MOV R1, R0");
            pop_r1();
            store(lt);
            return;
        }
        case ND_ASSIGN:
            if (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION) {
                int size = node->lhs->ty->size;
                if (node->rhs->kind == ND_FUNCALL &&
                    (node->rhs->ty->kind == TY_STRUCT || node->rhs->ty->kind == TY_UNION)) {
                    gen_addr(ctx, node->lhs);
                    emitln("    MOV R0, R3");
                    gen_expr(ctx, node->rhs); /* src addr in R0 */
                    emitln("    MOV R3, R2"); /* dst */
                } else {
                    gen_addr(ctx, node->lhs);
                    emitln("    MOV R0, R2"); /* dst */
                    gen_addr(ctx, node->rhs);
                }
                emitln("    MOV #%d, R1", size);
                emitln("    MOV R1, -(R6)");
                emitln("    MOV R0, -(R6)");
                emitln("    MOV R2, -(R6)");
                emitln("    JSR R5, __memcpy");
                emitln("    ADD #6, R6");
                if (node->rhs->kind == ND_FUNCALL &&
                    (node->rhs->ty->kind == TY_STRUCT || node->rhs->ty->kind == TY_UNION)) {
                    emitln("    ADD #%d, R6", size); /* pop temp */
                }
                return;
            }
            gen_addr(ctx, node->lhs);
            push_r0();
            gen_expr(ctx, node->rhs);
            pop_r1();
            store(node->lhs->ty);
            return;
        case ND_NEG:
            gen_expr(ctx, node->lhs);
            emitln("    MOV R0, R1");
            emitln("    CLR R0");
            emitln("    SUB R1, R0");
            return;
        case ND_BITNOT:
            gen_expr(ctx, node->lhs);
            emitln("    MOV R0, -(R6)");
            emitln("    JSR R5, __not");
            emitln("    ADD #2, R6");
            return;
        case ND_SIZEOF:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_CAST:
            gen_expr(ctx, node->lhs);
            if (node->ty && node->ty->size == 1) {
                emitln("    BIC #0xFF00, R0");
            }
            return;
        case ND_ADD:
        case ND_SUB: {
            Type *lt = node->lhs->ty;
            Type *rt = node->rhs->ty;
            int scale = 1;
            int lptr = (lt->kind == TY_PTR || lt->kind == TY_ARRAY);
            int rptr = (rt->kind == TY_PTR || rt->kind == TY_ARRAY);

            if (lptr && rptr && node->kind == ND_SUB) {
                scale = lt->base->size;
                gen_expr(ctx, node->lhs);
                push_r0();
                gen_expr(ctx, node->rhs);
                pop_r1();
                emitln("    SUB R0, R1");
                if (scale == 2) emitln("    ASR R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (!lptr && rptr && node->kind == ND_ADD) {
                scale = rt->base->size;
                gen_expr(ctx, node->rhs);
                push_r0();
                gen_expr(ctx, node->lhs);
                if (scale == 2) emitln("    ADD R0, R0");
                pop_r1();
                emitln("    ADD R0, R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (lptr && !rptr) scale = lt->base->size;

            gen_expr(ctx, node->lhs);
            push_r0();
            gen_expr(ctx, node->rhs);
            if (lptr && scale == 2) emitln("    ADD R0, R0");
            pop_r1();
            if (node->kind == ND_ADD)
                emitln("    ADD R0, R1");
            else
                emitln("    SUB R0, R1");
            emitln("    MOV R1, R0");
            return;
        }
        case ND_MUL:
        case ND_DIV:
        case ND_MOD: {
            gen_expr(ctx, node->rhs);
            push_r0();
            gen_expr(ctx, node->lhs);
            push_r0();
            if (node->kind == ND_MUL)
                emitln("    JSR R5, __mul");
            else if (node->kind == ND_DIV)
                emitln("    JSR R5, __div");
            else
                emitln("    JSR R5, __mod");
            emitln("    ADD #4, R6");
            return;
        }
        case ND_BITAND:
        case ND_BITOR:
        case ND_BITXOR:
        case ND_SHL:
        case ND_SHR: {
            gen_expr(ctx, node->rhs);
            push_r0();
            gen_expr(ctx, node->lhs);
            push_r0();
            if (node->kind == ND_BITAND)
                emitln("    JSR R5, __and");
            else if (node->kind == ND_BITOR)
                emitln("    JSR R5, __or");
            else if (node->kind == ND_BITXOR)
                emitln("    JSR R5, __xor");
            else if (node->kind == ND_SHL)
                emitln("    JSR R5, __shl");
            else
                emitln("    JSR R5, __shr");
            emitln("    ADD #4, R6");
            return;
        }
        case ND_EQ:
            gen_cmp(ctx, node, ND_EQ);
            return;
        case ND_NE:
            gen_cmp(ctx, node, ND_NE);
            return;
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE:
            gen_rel(ctx, node, node->kind);
            return;
        case ND_LOGAND:
            c = next_label(ctx);
            gen_expr(ctx, node->lhs);
            emitln("    TST R0");
            emit_beq(ctx, ".Lfalse%d", c);
            gen_expr(ctx, node->rhs);
            emitln("    TST R0");
            emit_beq(ctx, ".Lfalse%d", c);
            emitln("    MOV #1, R0");
            emit_br(ctx, ".Lend%d", c);
            emitln(".Lfalse%d:", c);
            emitln("    MOV #0, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_LOGOR:
            c = next_label(ctx);
            gen_expr(ctx, node->lhs);
            emitln("    TST R0");
            emit_bne(ctx, ".Ltrue%d", c);
            gen_expr(ctx, node->rhs);
            emitln("    TST R0");
            emit_bne(ctx, ".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(ctx, ".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_NOT:
            c = next_label(ctx);
            gen_expr(ctx, node->lhs);
            emitln("    TST R0");
            emit_beq(ctx, ".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(ctx, ".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_TERNARY:
            c = next_label(ctx);
            gen_expr(ctx, node->cond);
            emitln("    TST R0");
            emit_beq(ctx, ".Lelse%d", c);
            gen_expr(ctx, node->then);
            emit_br(ctx, ".Lend%d", c);
            emitln(".Lelse%d:", c);
            gen_expr(ctx, node->els);
            emitln(".Lend%d:", c);
            return;
        case ND_FUNCALL: {
            Node *arg;
            int nargs = 0;
            Node *args[32];
            int is_sret = (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION);
            int size = is_sret ? node->ty->size : 0;

            for (arg = node->args; arg; arg = arg->next) {
                args[nargs++] = arg;
            }

            if (is_sret) {
                emitln("    SUB #%d, R6", size);
                emitln("    MOV R6, R2");
            }

            while (nargs--) {
                gen_expr(ctx, args[nargs]);
                push_r0();
            }
            if (is_sret) {
                emitln("    MOV R2, R0");
                push_r0();
            }
            emitln("    JSR R5, %s", node->var->asm_name);
            if (node->args) {
                int count = 0;
                for (arg = node->args; arg; arg = arg->next) count++;
                emitln("    ADD #%d, R6", (count + (is_sret ? 1 : 0)) * 2);
            } else if (is_sret) {
                emitln("    ADD #2, R6");
            }
            if (is_sret) {
                emitln("    MOV R6, R0");
            }
            return;
        }
    }
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
                        gen_addr(ctx, node->lhs);
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
