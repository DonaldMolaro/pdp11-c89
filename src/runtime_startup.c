#include "runtime.h"

#include "emitter.h"

void runtime_emit_startup(void) {
    emit_label("_start");
    emitln("    MOV #0, R0");
    emitln("    TRAP #26"); /* select data bank 0 */
    emitln("    MOV #__data_start, R1");
    emitln("    MOV #__data_end, R2");
    emit_label(".Ldata_copy");
    emitln("    CMP R1, R2");
    emitln("    BEQ .Ldata_copy_done");
    emitln("    MOV (R1), R3"); /* read from bank 0 */
    emitln("    MOV #1, R0");
    emitln("    TRAP #26"); /* switch to bank 1 */
    emitln("    MOV R3, (R1)"); /* write to bank 1 */
    emitln("    MOV #0, R0");
    emitln("    TRAP #26"); /* back to bank 0 */
    emitln("    ADD #2, R1");
    emitln("    BR .Ldata_copy");
    emit_label(".Ldata_copy_done");
    emitln("    MOV #1, R0");
    emitln("    TRAP #26"); /* leave data bank at 1 */
    emitln("    MOV #0xFFFE, R6");
    emitln("    JSR R5, main");
    emitln("    HALT");
}

void runtime_emit_data(void) {
    emitln("stdin:");
    emitln("    .WORD 0x0000");
    emitln("stdout:");
    emitln("    .WORD 0x0000");
    emitln("stderr:");
    emitln("    .WORD 0x0000");
    emitln("__rt_fgetc_buf:");
    emitln("    .WORD 0x0000");
    emitln("__rt_ungetc_fp:");
    emitln("    .WORD 0x0000");
    emitln("__rt_ungetc_ch:");
    emitln("    .WORD 0x0000");
    emitln("__rt_ungetc_has:");
    emitln("    .WORD 0x0000");
    emitln("__rt_feof_flag:");
    emitln("    .WORD 0x0000");
    emitln("__rt_ferror_flag:");
    emitln("    .WORD 0x0000");
}
