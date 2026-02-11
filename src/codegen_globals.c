#include "codegen_internal.h"
#include "emitter.h"
#include "runtime.h"

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

void codegen_emit_globals(Obj *prog) {
    Obj *var;

    emit_label("__data_start");
    for (var = prog; var; var = var->next) {
        if (var->is_function) continue;
        emit_global_data(var);
    }
    runtime_emit_data();
    emit_label("__data_end");
}
