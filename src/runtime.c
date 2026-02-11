#include "runtime.h"

#include "emitter.h"

static void emit_data_load(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOV (%s), %s", tmp, reg);
}

static void emit_data_load_byte(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOVB (%s), %s", tmp, reg);
}

static void emit_data_store(const char *label, const char *reg, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    MOV %s, (%s)", reg, tmp);
}

static void emit_data_clr(const char *label, const char *tmp) {
    emitln("    MOV #%s, %s", label, tmp);
    emitln("    CLR (%s)", tmp);
}

void runtime_emit_startup(void) {
    emitln("_start:");
    emitln("    MOV #0, R0");
    emitln("    TRAP #26"); /* select data bank 0 */
    emitln("    MOV #__data_start, R1");
    emitln("    MOV #__data_end, R2");
    emitln(".Ldata_copy:");
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
    emitln(".Ldata_copy_done:");
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

void runtime_emit_functions(void) {
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

    emitln("print_uint:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #7");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("print_int:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #4");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("print_hex:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #6");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("scanf_int:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    TRAP #9");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("scanf_hex:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    TRAP #10");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("readline:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    TRAP #5");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("printf:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV #1, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, vfprintf");
    emitln("    ADD #4, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fprintf:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 6(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, vfprintf");
    emitln("    ADD #4, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("sprintf:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    MOV 6(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, vsprintf");
    emitln("    ADD #4, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("vfprintf:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    SUB #2, R6"); /* count */
    emitln("    CLR -2(R4)");
    emitln("    MOV 6(R4), R1"); /* fmt */
    emitln("    MOV 8(R4), R2"); /* ap */
    emitln(".Lfmt_loop:");
    emitln("    MOVB (R1)+, R0");
    emitln("    BEQ .Lfmt_done");
    emitln("    CMP #0x0025, R0"); /* '%' */
    emitln("    BEQ .Lfmt_conv");
    emitln("    MOV 4(R4), R3"); /* fp */
    emitln("    MOV R0, -(R6)");
    emitln("    MOV R3, -(R6)");
    emitln("    JSR R5, fputc");
    emitln("    ADD #4, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_conv:");
    emitln("    MOVB (R1)+, R0");
    emitln("    BEQ .Lfmt_done");
    emitln("    CMP #0x0064, R0"); /* 'd' */
    emitln("    BEQ .Lfmt_int");
    emitln("    CMP #0x0075, R0"); /* 'u' */
    emitln("    BEQ .Lfmt_uint");
    emitln("    CMP #0x0078, R0"); /* 'x' */
    emitln("    BEQ .Lfmt_hex");
    emitln("    CMP #0x0063, R0"); /* 'c' */
    emitln("    BEQ .Lfmt_char");
    emitln("    CMP #0x0073, R0"); /* 's' */
    emitln("    BEQ .Lfmt_str");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_int:");
    emitln("    MOV (R2)+, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, print_int");
    emitln("    ADD #2, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_uint:");
    emitln("    MOV (R2)+, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, print_uint");
    emitln("    ADD #2, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_hex:");
    emitln("    MOV (R2)+, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, print_hex");
    emitln("    ADD #2, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_char:");
    emitln("    MOV (R2)+, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    MOV 4(R4), R1");
    emitln("    MOV R1, -(R6)");
    emitln("    JSR R5, fputc");
    emitln("    ADD #4, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_str:");
    emitln("    MOV (R2)+, R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, puts");
    emitln("    ADD #2, R6");
    emitln("    INC -2(R4)");
    emitln("    BR .Lfmt_loop");
    emitln(".Lfmt_done:");
    emitln("    MOV -2(R4), R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("vsprintf:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    MOV 6(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, vfprintf");
    emitln("    ADD #4, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fopen:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* path */
    emitln("    MOV 6(R4), R1"); /* mode string */
    emitln("    MOVB (R1), R2");
    emitln("    CMP #0x0072, R2"); /* 'r' */
    emitln("    BEQ .Lfopen_r");
    emitln("    CMP #0x0077, R2"); /* 'w' */
    emitln("    BEQ .Lfopen_w");
    emitln("    CMP #0x0061, R2"); /* 'a' */
    emitln("    BEQ .Lfopen_a");
    emitln("    CLR R1");
    emitln("    BR .Lfopen_mode");
    emitln(".Lfopen_r:");
    emitln("    CLR R1");
    emitln("    BR .Lfopen_chkplus");
    emitln(".Lfopen_w:");
    emitln("    MOV #1, R1");
    emitln("    BR .Lfopen_chkplus");
    emitln(".Lfopen_a:");
    emitln("    MOV #2, R1");
    emitln(".Lfopen_chkplus:");
    emitln("    MOV 6(R4), R2");
    emitln("    MOVB 1(R2), R2");
    emitln("    CMP #0x002B, R2"); /* '+' */
    emitln("    BNE .Lfopen_mode");
    emitln("    MOV #3, R1");
    emitln(".Lfopen_mode:");
    emitln("    TRAP #20");
    emitln("    CMP #0xFFFF, R0");
    emitln("    BEQ .Lfopen_fail");
    emitln("    INC R0"); /* make handle non-NULL */
    emitln("    BR .Lfopen_ret");
    emitln(".Lfopen_fail:");
    emitln("    CLR R0");
    emitln(".Lfopen_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fread:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 6(R4), R2"); /* size */
    emitln("    MOV 8(R4), R3"); /* nmemb */
    emitln("    TST R2");
    emitln("    BEQ .Lfread_zero");
    emitln("    TST R3");
    emitln("    BEQ .Lfread_zero");
    emitln("    MOV R3, -(R6)");
    emitln("    MOV R2, -(R6)");
    emitln("    JSR R5, __mul"); /* size * nmemb */
    emitln("    ADD #4, R6");
    emitln("    MOV R0, R2"); /* total bytes */
    emitln("    MOV 10(R4), R0"); /* handle */
    emitln("    TST R0");
    emitln("    BEQ .Lfread_zero");
    emitln("    DEC R0"); /* back to handle */
    emitln("    MOV 4(R4), R1"); /* buf */
    emitln("    TRAP #21");
    emitln("    MOV 6(R4), R1"); /* size */
    emitln("    CMP #1, R1");
    emitln("    BEQ .Lfread_ret");
    emitln("    MOV R1, -(R6)");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, __div"); /* bytes / size */
    emitln("    ADD #4, R6");
    emitln(".Lfread_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Lfread_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fwrite:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 6(R4), R2"); /* size */
    emitln("    MOV 8(R4), R3"); /* nmemb */
    emitln("    TST R2");
    emitln("    BEQ .Lfwrite_zero");
    emitln("    TST R3");
    emitln("    BEQ .Lfwrite_zero");
    emitln("    MOV R3, -(R6)");
    emitln("    MOV R2, -(R6)");
    emitln("    JSR R5, __mul");
    emitln("    ADD #4, R6");
    emitln("    MOV R0, R2"); /* total bytes */
    emitln("    MOV 10(R4), R0"); /* handle */
    emitln("    TST R0");
    emitln("    BEQ .Lfwrite_zero");
    emitln("    DEC R0"); /* back to handle */
    emitln("    MOV 4(R4), R1"); /* buf */
    emitln("    TRAP #22");
    emitln("    MOV 6(R4), R1"); /* size */
    emitln("    CMP #1, R1");
    emitln("    BEQ .Lfwrite_ret");
    emitln("    MOV R1, -(R6)");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, __div");
    emitln("    ADD #4, R6");
    emitln(".Lfwrite_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Lfwrite_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fclose:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* handle */
    emitln("    TST R0");
    emitln("    BEQ .Lfclose_bad");
    emitln("    DEC R0"); /* back to handle */
    emitln("    TRAP #23");
    emitln("    BR .Lfclose_ret");
    emitln(".Lfclose_bad:");
    emitln("    MOV #0xFFFF, R0");
    emitln(".Lfclose_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fseek:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* handle */
    emitln("    TST R0");
    emitln("    BEQ .Lfseek_bad");
    emitln("    DEC R0"); /* back to handle */
    emitln("    MOV 6(R4), R1"); /* offset */
    emitln("    MOV 8(R4), R2"); /* whence */
    emitln("    TRAP #24");
    emitln("    BR .Lfseek_ret");
    emitln(".Lfseek_bad:");
    emitln("    MOV #0xFFFF, R0");
    emitln(".Lfseek_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("ftell:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* handle */
    emitln("    TST R0");
    emitln("    BEQ .Lftell_bad");
    emitln("    DEC R0"); /* back to handle */
    emitln("    TRAP #25");
    emitln("    BR .Lftell_ret");
    emitln(".Lftell_bad:");
    emitln("    MOV #0xFFFF, R0");
    emitln(".Lftell_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("pdp11_set_bank:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* bank */
    emitln("    TRAP #26");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fgetc:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* fp */
    emit_data_load("__rt_ungetc_has", "R1", "R2");
    emitln("    TST R1");
    emitln("    BEQ .Lfgetc_read");
    emit_data_load("__rt_ungetc_fp", "R1", "R2");
    emitln("    CMP R1, R0");
    emitln("    BNE .Lfgetc_read");
    emit_data_load("__rt_ungetc_ch", "R0", "R2");
    emit_data_clr("__rt_ungetc_has", "R2");
    emitln("    BR .Lfgetc_ret");
    emitln(".Lfgetc_read:");
    emitln("    TST R0");
    emitln("    BEQ .Lfgetc_stdin");
    emitln(".Lfgetc_file:");
    emitln("    MOV 4(R4), R0"); /* fp */
    emitln("    DEC R0"); /* handle */
    emitln("    MOV #__rt_fgetc_buf, R1");
    emitln("    MOV #1, R2");
    emitln("    TRAP #21");
    emitln("    CMP #1, R0");
    emitln("    BNE .Lfgetc_eof");
    emit_data_load_byte("__rt_fgetc_buf", "R0", "R1");
    emit_data_clr("__rt_feof_flag", "R1");
    emitln("    BR .Lfgetc_ret");
    emitln(".Lfgetc_eof:");
    emitln("    MOV #1, R0");
    emit_data_store("__rt_feof_flag", "R0", "R1");
    emitln("    MOV #0xFFFF, R0");
    emitln("    BR .Lfgetc_ret");
    emitln(".Lfgetc_stdin:");
    emitln("    TRAP #2");
    emitln("    CMP #0, R0");
    emitln("    BNE .Lfgetc_stdin_ok");
    emitln("    MOV #1, R0");
    emit_data_store("__rt_feof_flag", "R0", "R1");
    emitln("    MOV #0xFFFF, R0");
    emitln("    BR .Lfgetc_ret");
    emitln(".Lfgetc_stdin_ok:");
    emit_data_clr("__rt_feof_flag", "R1");
    emitln(".Lfgetc_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("getc:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, fgetc");
    emitln("    ADD #2, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fputc:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R1"); /* c */
    emitln("    MOV 6(R4), R0"); /* fp */
    emitln("    TST R0");
    emitln("    BEQ .Lfputc_stdout");
    emitln("    DEC R0"); /* handle */
    emitln("    SUB #2, R6");
    emitln("    MOV R6, R2"); /* buf */
    emitln("    MOVB R1, (R2)");
    emitln("    MOV #1, R3"); /* len */
    emitln("    MOV R2, R1");
    emitln("    MOV R3, R2");
    emitln("    TRAP #22");
    emitln("    ADD #2, R6");
    emitln("    CMP #0, R0");
    emitln("    BEQ .Lfputc_fail");
    emitln("    MOV 4(R4), R0");
    emitln("    BR .Lfputc_ret");
    emitln(".Lfputc_stdout:");
    emitln("    MOV 4(R4), R0");
    emitln("    TRAP #1");
    emitln("    MOV 4(R4), R0");
    emitln("    BR .Lfputc_ret");
    emitln(".Lfputc_fail:");
    emitln("    MOV #0xFFFF, R0");
    emitln(".Lfputc_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("putc:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R1, -(R6)");
    emitln("    MOV R0, -(R6)");
    emitln("    JSR R5, fputc");
    emitln("    ADD #4, R6");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fgets:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R1"); /* buf */
    emitln("    MOV 6(R4), R2"); /* n */
    emitln("    MOV 8(R4), R3"); /* fp */
    emitln("    TST R2");
    emitln("    BEQ .Lfgets_null");
    emitln("    CMP #1, R2");
    emitln("    BNE .Lfgets_setup");
    emitln("    CLRB (R1)");
    emitln("    MOV R1, R0");
    emitln("    BR .Lfgets_ret");
    emitln(".Lfgets_setup:");
    emitln("    SUB #1, R2"); /* remaining = n-1 */
    emitln("    SUB #2, R6");
    emitln("    CLR -2(R4)"); /* saw_char = 0 */
    emitln(".Lfgets_next:");
    emitln("    TST R2");
    emitln("    BEQ .Lfgets_done");
    emitln("    MOV R1, -(R6)");
    emitln("    MOV R2, -(R6)");
    emitln("    MOV R3, -(R6)");
    emitln("    JSR R5, fgetc");
    emitln("    ADD #2, R6");
    emitln("    MOV (R6)+, R2");
    emitln("    MOV (R6)+, R1");
    emitln("    CMP #0xFFFF, R0");
    emitln("    BEQ .Lfgets_eof");
    emitln("    MOV #1, -2(R4)"); /* saw_char = 1 */
    emitln("    MOVB R0, (R1)+");
    emitln("    DEC R2");
    emitln("    CMP #0x000A, R0"); /* '\\n' */
    emitln("    BEQ .Lfgets_done");
    emitln("    BR .Lfgets_next");
    emitln(".Lfgets_eof:");
    emitln("    TST -2(R4)");
    emitln("    BNE .Lfgets_done");
    emitln("    ADD #2, R6");
    emitln("    CLR R0");
    emitln("    BR .Lfgets_ret");
    emitln(".Lfgets_done:");
    emitln("    CLRB (R1)");
    emitln("    ADD #2, R6");
    emitln("    MOV 4(R4), R0");
    emitln("    BR .Lfgets_ret");
    emitln(".Lfgets_null:");
    emitln("    CLR R0");
    emitln(".Lfgets_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("ungetc:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* c */
    emitln("    CMP #0xFFFF, R0");
    emitln("    BEQ .Lungetc_ret");
    emitln("    MOV 6(R4), R1"); /* fp */
    emit_data_store("__rt_ungetc_fp", "R1", "R2");
    emit_data_store("__rt_ungetc_ch", "R0", "R2");
    emitln("    MOV #1, R1");
    emit_data_store("__rt_ungetc_has", "R1", "R2");
    emit_data_clr("__rt_feof_flag", "R2");
    emitln(".Lungetc_ret:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("feof:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emit_data_load("__rt_feof_flag", "R0", "R1");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("ferror:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emit_data_load("__rt_ferror_flag", "R0", "R1");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("fflush:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV #0, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__memcpy:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0"); /* dst */
    emitln("    MOV 6(R4), R1"); /* src */
    emitln("    MOV 8(R4), R2"); /* size */
    emitln(".Lmemcpy_loop:");
    emitln("    TST R2");
    emitln("    BEQ .Lmemcpy_end");
    emitln("    MOVB (R1)+, (R0)+");
    emitln("    DEC R2");
    emitln("    BR .Lmemcpy_loop");
    emitln(".Lmemcpy_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__not:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R1");
    emitln("    MOV #0xFFFF, R0");
    emitln("    BIC R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__and:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV #0xFFFF, R2");
    emitln("    BIC R0, R2");
    emitln("    MOV #0xFFFF, R3");
    emitln("    BIC R1, R3");
    emitln("    MOV R2, R0");
    emitln("    BIS R3, R0");
    emitln("    MOV #0xFFFF, R1");
    emitln("    BIC R0, R1");
    emitln("    MOV R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__or:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    BIS R1, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__xor:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R2");
    emitln("    BIC R1, R2");
    emitln("    MOV R1, R3");
    emitln("    BIC R0, R3");
    emitln("    BIS R2, R3");
    emitln("    MOV R3, R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__shl:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln(".Lshl_loop:");
    emitln("    TST R1");
    emitln("    BEQ .Lshl_end");
    emitln("    ASL R0");
    emitln("    DEC R1");
    emitln("    BR .Lshl_loop");
    emitln(".Lshl_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__shr:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln(".Lshr_loop:");
    emitln("    TST R1");
    emitln("    BEQ .Lshr_end");
    emitln("    ASR R0");
    emitln("    DEC R1");
    emitln("    BR .Lshr_loop");
    emitln(".Lshr_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__mul:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    CLR R2");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmul_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, R2");
    emitln(".Lmul_a_pos:");
    emitln("    MOV R1, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmul_b_pos");
    emitln("    MOV R1, R3");
    emitln("    CLR R1");
    emitln("    SUB R3, R1");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, R2");
    emitln(".Lmul_b_pos:");
    emitln("    CLR R3");
    emitln(".Lmul_loop:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Lmul_done");
    emitln("    ADD R0, R3");
    emitln("    DEC R1");
    emitln("    BR .Lmul_loop");
    emitln(".Lmul_done:");
    emitln("    MOV R3, R0");
    emitln("    CMP #0, R2");
    emitln("    BEQ .Lmul_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Lmul_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__div:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    SUB #2, R6");
    emitln("    CLR -2(R4)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV -2(R4), R2");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Ldiv_a_pos:");
    emitln("    MOV R1, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_b_pos");
    emitln("    MOV R1, R3");
    emitln("    CLR R1");
    emitln("    SUB R3, R1");
    emitln("    MOV -2(R4), R2");
    emitln("    MOV #1, R3");
    emitln("    SUB R2, R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Ldiv_b_pos:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Ldiv_zero");
    emitln("    CLR R2");
    emitln(".Ldiv_loop:");
    emitln("    MOV R0, R3");
    emitln("    SUB R1, R3");
    emitln("    MOV R3, R0");
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
    emitln("    CMP #0, R0");
    emitln("    BNE .Ldiv_done");
    emitln("    MOV R3, R0");
    emitln("    INC R2");
    emitln("    BR .Ldiv_loop");
    emitln(".Ldiv_done:");
    emitln("    MOV R2, R0");
    emitln("    MOV -2(R4), R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Ldiv_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Ldiv_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Ldiv_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");

    emitln("__mod:");
    emitln("    MOV R4, -(R6)");
    emitln("    MOV R6, R4");
    emitln("    SUB #2, R6");
    emitln("    CLR -2(R4)");
    emitln("    MOV 4(R4), R0");
    emitln("    MOV 6(R4), R1");
    emitln("    MOV R0, R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    ASR R3");
    emitln("    BIC #0xFFFE, R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmod_a_pos");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln("    MOV #1, R3");
    emitln("    SUB -2(R4), R3");
    emitln("    MOV R3, -2(R4)");
    emitln(".Lmod_a_pos:");
    emitln("    CMP #0, R1");
    emitln("    BEQ .Lmod_zero");
    emitln(".Lmod_loop:");
    emitln("    MOV R0, R3");
    emitln("    SUB R1, R3");
    emitln("    MOV R3, R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    ASR R2");
    emitln("    BIC #0xFFFE, R2");
    emitln("    CMP #0, R2");
    emitln("    BNE .Lmod_done");
    emitln("    MOV R3, R0");
    emitln("    BR .Lmod_loop");
    emitln(".Lmod_done:");
    emitln("    MOV -2(R4), R3");
    emitln("    CMP #0, R3");
    emitln("    BEQ .Lmod_end");
    emitln("    MOV R0, R3");
    emitln("    CLR R0");
    emitln("    SUB R3, R0");
    emitln(".Lmod_end:");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
    emitln(".Lmod_zero:");
    emitln("    CLR R0");
    emitln("    MOV R4, R6");
    emitln("    MOV (R6)+, R4");
    emitln("    RTS R5");
}
