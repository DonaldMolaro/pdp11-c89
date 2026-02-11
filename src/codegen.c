#include "codegen_internal.h"
#include "emitter.h"
#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    CodegenContext ctx;

    memset(&ctx, 0, sizeof(ctx));
    emit_init(outfp);
    emitln(".ORIG 0x1000");
    runtime_emit_startup();

    for (fn = prog; fn; fn = fn->next) {
        if (!fn->is_function) continue;
        if (!fn->body) continue;
        ctx.current_fn = fn->asm_name;
        ctx.current_fn_obj = fn;
        emitln("%s:", fn->asm_name);
        emitln("    MOV R4, -(R6)");
        emitln("    MOV R6, R4");
        if (fn->stack_size)
            emitln("    SUB #%d, R6", fn->stack_size);
        gen_stmt(&ctx, fn->body);
        emitln(".Lreturn_%s:", fn->asm_name);
        emitln("    MOV R4, R6");
        emitln("    MOV (R6)+, R4");
        emitln("    RTS R5");
    }

    emit_label("__data_start");
    for (fn = prog; fn; fn = fn->next) {
        if (fn->is_function) continue;
        emit_global_data(fn);
    }
    runtime_emit_data();
    emit_label("__data_end");

    runtime_emit_functions();
}
