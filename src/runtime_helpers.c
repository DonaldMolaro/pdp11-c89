#include "runtime_helpers.h"

#include "emitter.h"

void emit_data_load(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOV (%s), %s", tmp, reg);
}

void emit_data_load_byte(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOVB (%s), %s", tmp, reg);
}

void emit_data_store(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOV %s, (%s)", reg, tmp);
}

void emit_data_clr(const char *label, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    CLR (%s)", tmp);
}
