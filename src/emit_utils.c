#include "emit_utils.h"

#include "emitter.h"

void emit_push(const char *reg) {
    emitln("    MOV %s, -(R6)", reg);
}

void emit_pop(const char *reg) {
    emitln("    MOV (R6)+, %s", reg);
}
