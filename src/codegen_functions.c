#include "codegen_internal.h"
#include "emitter.h"

#include <string.h>

void codegen_emit_functions(Obj *prog) {
    Obj *fn;
    CodegenContext ctx;

    memset(&ctx, 0, sizeof(ctx));
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
}
