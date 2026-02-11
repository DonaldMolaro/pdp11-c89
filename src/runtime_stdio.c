#include "runtime.h"
#include "emitter.h"

void runtime_emit_stdio(void) {
    emitln("putchar:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #1");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("getchar:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    TRAP #2");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("puts:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #3");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
}
