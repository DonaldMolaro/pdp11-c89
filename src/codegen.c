#include "compiler.h"
#include "emitter.h"
#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int labelseq;
static char *current_fn;
static Obj *current_fn_obj;
static int break_labels[32];
static int continue_labels[32];
static int break_depth;
static int continue_depth;

static void emit_br_label(const char *label) {
    emitln("    JMP %s", label);
}

static void emit_beq_label(const char *label) {
    int c = labelseq++;
    emitln("    BNE .Lskip%d", c);
    emitln("    JMP %s", label);
    emitln(".Lskip%d:", c);
}

static void emit_bne_label(const char *label) {
    int c = labelseq++;
    emitln("    BEQ .Lskip%d", c);
    emitln("    JMP %s", label);
    emitln(".Lskip%d:", c);
}

static void emit_br(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_br_label(buf);
}

static void emit_beq(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_beq_label(buf);
}

static void emit_bne(const char *fmt, int n) {
    char buf[64];
    sprintf(buf, fmt, n);
    emit_bne_label(buf);
}

static void emit_br_name(const char *fmt, const char *name) {
    char buf[64];
    sprintf(buf, fmt, name);
    emit_br_label(buf);
}

static void gen_expr(Node *node);
static void gen_stmt(Node *node);
static void gen_addr(Node *node);

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

static void push_break(int label) {
    break_labels[break_depth++] = label;
}

static void pop_break(void) {
    if (break_depth > 0) break_depth--;
}

static void push_continue(int label) {
    continue_labels[continue_depth++] = label;
}

static void pop_continue(void) {
    if (continue_depth > 0) continue_depth--;
}

static int current_break(void) {
    return break_depth ? break_labels[break_depth - 1] : -1;
}

static int current_continue(void) {
    return continue_depth ? continue_labels[continue_depth - 1] : -1;
}

static void gen_addr(Node *node) {
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
        gen_expr(node->lhs);
        return;
    }
    if (node->kind == ND_MEMBER) {
        gen_addr(node->lhs);
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

static void gen_cmp(Node *node, int kind) {
    int c = labelseq++;

    gen_expr(node->lhs);
    push_r0();
    gen_expr(node->rhs);
    pop_r1();

    emitln("    CMP R0, R1");
    if (kind == ND_EQ)
        emit_beq(".Ltrue%d", c);
    else
        emit_bne(".Ltrue%d", c);

    emitln("    MOV #0, R0");
    emit_br(".Lend%d", c);
    emitln(".Ltrue%d:", c);
    emitln("    MOV #1, R0");
    emitln(".Lend%d:", c);
}

static void gen_rel(Node *node, int kind) {
    int c = labelseq++;

    gen_expr(node->lhs);
    push_r0();
    gen_expr(node->rhs);
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
    emit_beq(".Lzero%d", c);
    emitln("    MOV #0, R1");
    emit_br(".Lzero_end%d", c);
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

static void gen_expr(Node *node) {
    int c;

    switch (node->kind) {
        case ND_NUM:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_VAR:
            gen_addr(node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_MEMBER:
            gen_addr(node);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_ADDR:
            gen_addr(node->lhs);
            return;
        case ND_DEREF:
            gen_expr(node->lhs);
            if (node->ty->kind == TY_ARRAY) return;
            load(node->ty);
            return;
        case ND_PRE_INC: {
            int inc = inc_amount(node->lhs->ty);
            gen_addr(node->lhs);
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
            gen_addr(node->lhs);
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
            gen_addr(node->lhs);
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
            gen_addr(node->lhs);
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

            gen_addr(node->lhs);
            push_r0();
            load(lt);
            push_r0();
            gen_expr(node->rhs);
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
                    gen_addr(node->lhs);
                    emitln("    MOV R0, R3");
                    gen_expr(node->rhs); /* src addr in R0 */
                    emitln("    MOV R3, R2"); /* dst */
                } else {
                    gen_addr(node->lhs);
                    emitln("    MOV R0, R2"); /* dst */
                    gen_addr(node->rhs);
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
            gen_addr(node->lhs);
            push_r0();
            gen_expr(node->rhs);
            pop_r1();
            store(node->lhs->ty);
            return;
        case ND_NEG:
            gen_expr(node->lhs);
            emitln("    MOV R0, R1");
            emitln("    CLR R0");
            emitln("    SUB R1, R0");
            return;
        case ND_BITNOT:
            gen_expr(node->lhs);
            emitln("    MOV R0, -(R6)");
            emitln("    JSR R5, __not");
            emitln("    ADD #2, R6");
            return;
        case ND_SIZEOF:
            emitln("    MOV #%ld, R0", node->val);
            return;
        case ND_CAST:
            gen_expr(node->lhs);
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
                gen_expr(node->lhs);
                push_r0();
                gen_expr(node->rhs);
                pop_r1();
                emitln("    SUB R0, R1");
                if (scale == 2) emitln("    ASR R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (!lptr && rptr && node->kind == ND_ADD) {
                scale = rt->base->size;
                gen_expr(node->rhs);
                push_r0();
                gen_expr(node->lhs);
                if (scale == 2) emitln("    ADD R0, R0");
                pop_r1();
                emitln("    ADD R0, R1");
                emitln("    MOV R1, R0");
                return;
            }

            if (lptr && !rptr) scale = lt->base->size;

            gen_expr(node->lhs);
            push_r0();
            gen_expr(node->rhs);
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
            gen_expr(node->rhs);
            push_r0();
            gen_expr(node->lhs);
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
            gen_expr(node->rhs);
            push_r0();
            gen_expr(node->lhs);
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
            gen_cmp(node, ND_EQ);
            return;
        case ND_NE:
            gen_cmp(node, ND_NE);
            return;
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE:
            gen_rel(node, node->kind);
            return;
        case ND_LOGAND:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_beq(".Lfalse%d", c);
            gen_expr(node->rhs);
            emitln("    TST R0");
            emit_beq(".Lfalse%d", c);
            emitln("    MOV #1, R0");
            emit_br(".Lend%d", c);
            emitln(".Lfalse%d:", c);
            emitln("    MOV #0, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_LOGOR:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_bne(".Ltrue%d", c);
            gen_expr(node->rhs);
            emitln("    TST R0");
            emit_bne(".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_NOT:
            c = labelseq++;
            gen_expr(node->lhs);
            emitln("    TST R0");
            emit_beq(".Ltrue%d", c);
            emitln("    MOV #0, R0");
            emit_br(".Lend%d", c);
            emitln(".Ltrue%d:", c);
            emitln("    MOV #1, R0");
            emitln(".Lend%d:", c);
            return;
        case ND_TERNARY:
            c = labelseq++;
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lelse%d", c);
            gen_expr(node->then);
            emit_br(".Lend%d", c);
            emitln(".Lelse%d:", c);
            gen_expr(node->els);
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
                gen_expr(args[nargs]);
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

static void gen_stmt(Node *node) {
    int c;

    switch (node->kind) {
        case ND_RETURN:
            if (current_fn_obj && current_fn_obj->has_sret) {
                if (node->lhs) {
                    int size = current_fn_obj->ty->return_ty->size;
                    if (node->lhs->kind == ND_FUNCALL &&
                        (node->lhs->ty->kind == TY_STRUCT || node->lhs->ty->kind == TY_UNION)) {
                        emitln("    MOV 4(R4), R3");
                        gen_expr(node->lhs); /* src in R0 */
                        emitln("    MOV R3, R2");
                    } else {
                        emitln("    MOV 4(R4), R2");
                        gen_addr(node->lhs);
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
                emit_br_name(".Lreturn_%s", current_fn);
                return;
            }
            if (node->lhs) gen_expr(node->lhs);
            emit_br_name(".Lreturn_%s", current_fn);
            return;
        case ND_IF:
            c = labelseq++;
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lelse%d", c);
            gen_stmt(node->then);
            emit_br(".Lend%d", c);
            emitln(".Lelse%d:", c);
            if (node->els) gen_stmt(node->els);
            emitln(".Lend%d:", c);
            return;
        case ND_WHILE:
            c = labelseq++;
            emitln(".Lbegin%d:", c);
            emitln(".Lcontinue%d:", c);
            gen_expr(node->cond);
            emitln("    TST R0");
            emit_beq(".Lend%d", c);
            push_break(c);
            push_continue(c);
            gen_stmt(node->body);
            pop_continue();
            pop_break();
            emit_br(".Lbegin%d", c);
            emitln(".Lend%d:", c);
            return;
        case ND_FOR: {
            int begin = labelseq++;
            int end = labelseq++;
            int cont = labelseq++;
            if (node->init) {
                if (node->init->kind == ND_BLOCK || node->init->kind == ND_EXPR_STMT || node->init->kind == ND_NULL)
                    gen_stmt(node->init);
                else
                    gen_expr(node->init);
            }
            emitln(".Lbegin%d:", begin);
            if (node->cond) {
                gen_expr(node->cond);
                emitln("    TST R0");
                emit_beq(".Lend%d", end);
            }
            push_break(end);
            push_continue(cont);
            gen_stmt(node->body);
            emitln(".Lcontinue%d:", cont);
            if (node->inc) gen_expr(node->inc);
            pop_continue();
            pop_break();
            emit_br(".Lbegin%d", begin);
            emitln(".Lend%d:", end);
            return;
        }
        case ND_BREAK:
            if (current_break() >= 0)
                emit_br(".Lend%d", current_break());
            return;
        case ND_CONTINUE:
            if (current_continue() >= 0)
                emit_br(".Lcontinue%d", current_continue());
            return;
        case ND_SWITCH: {
            int end = labelseq++;
            Node *cnode;
            gen_expr(node->cond);
            for (cnode = node->case_next; cnode; cnode = cnode->case_next) {
                cnode->case_label = labelseq++;
                emitln("    CMP #%ld, R0", cnode->val);
                emit_beq(".Lcase%d", cnode->case_label);
            }
            if (node->default_case) {
                node->default_case->case_label = labelseq++;
                emit_br(".Lcase%d", node->default_case->case_label);
            } else {
                emit_br(".Lend%d", end);
            }
            push_break(end);
            gen_stmt(node->body);
            pop_break();
            emitln(".Lend%d:", end);
            return;
        }
        case ND_CASE:
            emitln(".Lcase%d:", node->case_label);
            if (node->body) gen_stmt(node->body);
            return;
        case ND_BLOCK: {
            Node *stmt;
            for (stmt = node->body; stmt; stmt = stmt->next) {
                gen_stmt(stmt);
            }
            return;
        }
        case ND_EXPR_STMT:
            if (node->lhs) gen_expr(node->lhs);
            return;
        case ND_NULL:
            return;
    }
}

static void emit_global_data(Obj *var) {
    int total = var->ty->size;
    int i = 0;
    if (var->is_extern && !var->init_data) return;
    emitln("%s:", var->asm_name);
    while (i < total) {
        int lo = 0;
        int hi = 0;
        if (var->init_data && i < var->init_len) lo = (unsigned char)var->init_data[i];
        if (var->init_data && i + 1 < var->init_len) hi = (unsigned char)var->init_data[i + 1];
        emitln("    .WORD 0x%04X", (hi << 8) | lo);
        i += 2;
    }
}

void codegen(Obj *prog, FILE *outfp) {
    Obj *fn;

    emit_init(outfp);
    emitln(".ORIG 0x1000");
    runtime_emit_startup();

    for (fn = prog; fn; fn = fn->next) {
        if (!fn->is_function) continue;
        if (!fn->body) continue;
        current_fn = fn->asm_name;
        current_fn_obj = fn;
        emitln("%s:", fn->asm_name);
        emitln("    MOV R4, -(R6)");
        emitln("    MOV R6, R4");
        if (fn->stack_size)
            emitln("    SUB #%d, R6", fn->stack_size);
        gen_stmt(fn->body);
        emitln(".Lreturn_%s:", fn->asm_name);
        emitln("    MOV R4, R6");
        emitln("    MOV (R6)+, R4");
        emitln("    RTS R5");
    }

    emitln("__data_start:");
    for (fn = prog; fn; fn = fn->next) {
        if (fn->is_function) continue;
        emit_global_data(fn);
    }
    runtime_emit_data();
    emitln("__data_end:");

    runtime_emit_functions();
}
