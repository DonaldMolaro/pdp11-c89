#include "codegen_internal.h"
#include "emitter.h"
#include "runtime.h"

#include <stdio.h>
#include <string.h>

void codegen_emit_functions(Obj *prog);
void codegen_emit_globals(Obj *prog);

void codegen(Obj *prog, FILE *outfp) {
    emit_init(outfp);
    emitln(".ORIG 0x1000");
    runtime_emit_startup();
    codegen_emit_functions(prog);
    codegen_emit_globals(prog);
    runtime_emit_functions();
}
