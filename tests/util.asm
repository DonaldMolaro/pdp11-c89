.ORIG 0x1000
_start:
    MOV #0xFFFE, R6
    JSR R5, main
    HALT
set_current_input:
    MOV R4, -(R6)
    MOV R6, R4
    MOV #current_input, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV (R6)+, R1
    MOV R0, (R1)
.Lreturn_set_current_input:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
startswith:
    MOV R4, -(R6)
    MOV R6, R4
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, strlen
    ADD #2, R6
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, strncmp
    ADD #6, R6
    MOV R0, -(R6)
    MOV #0, R0
    MOV (R6)+, R1
    CMP R0, R1
    BEQ .Ltrue0
    MOV #0, R0
    BR .Lend0
.Ltrue0:
    MOV #1, R0
.Lend0:
    BR .Lreturn_startswith
.Lreturn_startswith:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
strndup2:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #2, R6
    MOV R4, R0
    ADD #-2, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #1, R0
    MOV (R6)+, R1
    ADD R0, R1
    MOV R1, R0
    MOV R0, -(R6)
    MOV #1, R0
    MOV R0, -(R6)
    JSR R5, xcalloc
    ADD #4, R6
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, memcpy
    ADD #6, R6
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV (R6)+, R1
    ADD R0, R1
    MOV R1, R0
    MOV R0, -(R6)
    MOV #48, R0
    MOV (R6)+, R1
    MOVB R0, (R1)
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    BR .Lreturn_strndup2
.Lreturn_strndup2:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
xcalloc:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #2, R6
    MOV R4, R0
    ADD #-2, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, calloc
    ADD #4, R6
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    TST R0
    BEQ .Ltrue2
    MOV #0, R0
    BR .Lend2
.Ltrue2:
    MOV #1, R0
.Lend2:
    TST R0
    BEQ .Lelse1
    MOV #.L.str5, R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, fprintf
    ADD #4, R6
    MOV #1, R0
    MOV R0, -(R6)
    JSR R5, exit
    ADD #2, R6
    BR .Lend1
.Lelse1:
.Lend1:
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    BR .Lreturn_xcalloc
.Lreturn_xcalloc:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
error_at:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #4, R6
    MOV R4, R0
    ADD #-2, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #current_input, R0
    MOV (R0), R0
    MOV (R6)+, R1
    SUB R0, R1
    MOV R1, R0
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV #current_input, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #.L.str0, R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, fprintf
    ADD #6, R6
    MOV #.L.str2, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #-2, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #.L.str1, R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, fprintf
    ADD #8, R6
    MOV #.L.str3, R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, fprintf
    ADD #4, R6
    MOV R4, R0
    ADD #-4, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV R0, -(R6)
    MOV #1, R0
    ADD R0, R0
    MOV (R6)+, R1
    ADD R0, R1
    MOV R1, R0
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV R4, R0
    ADD #-4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, vfprintf
    ADD #6, R6
    MOV #.L.str4, R0
    MOV R0, -(R6)
    MOV #stderr, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, fprintf
    ADD #4, R6
    MOV #1, R0
    MOV R0, -(R6)
    JSR R5, exit
    ADD #2, R6
.Lreturn_error_at:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.L.str5:
    .WORD 0x756F
    .WORD 0x2074
    .WORD 0x666F
    .WORD 0x6D20
    .WORD 0x6D65
    .WORD 0x726F
    .WORD 0x0A79
    .WORD 0x0000
.L.str4:
    .WORD 0x000A
.L.str3:
    .WORD 0x205E
    .WORD 0x0000
.L.str2:
    .WORD 0x0000
.L.str1:
    .WORD 0x2A25
    .WORD 0x0073
.L.str0:
    .WORD 0x7325
    .WORD 0x000A
current_input:
    .WORD 0x0000
stderr:
    .WORD 0x0000
stdout:
    .WORD 0x0000
stdin:
    .WORD 0x0000
putchar:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    TRAP #1
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
getchar:
    MOV R4, -(R6)
    MOV R6, R4
    TRAP #2
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
puts:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    TRAP #3
    MOV #0, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__memcpy:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOV 8(R4), R2
.Lmemcpy_loop:
    TST R2
    BEQ .Lmemcpy_end
    MOVB (R1)+, (R0)+
    DEC R2
    BR .Lmemcpy_loop
.Lmemcpy_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__not:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R1
    MOV #0xFFFF, R0
    BIC R1, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__and:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOV #0xFFFF, R2
    BIC R0, R2
    MOV #0xFFFF, R3
    BIC R1, R3
    MOV R2, R0
    BIS R3, R0
    MOV #0xFFFF, R1
    BIC R0, R1
    MOV R1, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__or:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    BIS R1, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__xor:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOV R0, R2
    BIC R1, R2
    MOV R1, R3
    BIC R0, R3
    BIS R2, R3
    MOV R3, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__shl:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
.Lshl_loop:
    TST R1
    BEQ .Lshl_end
    ASL R0
    DEC R1
    BR .Lshl_loop
.Lshl_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__shr:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
.Lshr_loop:
    TST R1
    BEQ .Lshr_end
    ASR R0
    DEC R1
    BR .Lshr_loop
.Lshr_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__mul:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    CLR R2
    MOV R0, R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    BIC #0xFFFE, R3
    CMP #0, R3
    BEQ .Lmul_a_pos
    MOV R0, R3
    CLR R0
    SUB R3, R0
    MOV #1, R3
    SUB R2, R3
    MOV R3, R2
.Lmul_a_pos:
    MOV R1, R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    BIC #0xFFFE, R3
    CMP #0, R3
    BEQ .Lmul_b_pos
    MOV R1, R3
    CLR R1
    SUB R3, R1
    MOV #1, R3
    SUB R2, R3
    MOV R3, R2
.Lmul_b_pos:
    CLR R3
.Lmul_loop:
    CMP #0, R1
    BEQ .Lmul_done
    ADD R0, R3
    DEC R1
    BR .Lmul_loop
.Lmul_done:
    MOV R3, R0
    CMP #0, R2
    BEQ .Lmul_end
    MOV R0, R3
    CLR R0
    SUB R3, R0
.Lmul_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__div:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #2, R6
    CLR -2(R4)
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOV R0, R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    BIC #0xFFFE, R3
    CMP #0, R3
    BEQ .Ldiv_a_pos
    MOV R0, R3
    CLR R0
    SUB R3, R0
    MOV -2(R4), R2
    MOV #1, R3
    SUB R2, R3
    MOV R3, -2(R4)
.Ldiv_a_pos:
    MOV R1, R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    BIC #0xFFFE, R3
    CMP #0, R3
    BEQ .Ldiv_b_pos
    MOV R1, R3
    CLR R1
    SUB R3, R1
    MOV -2(R4), R2
    MOV #1, R3
    SUB R2, R3
    MOV R3, -2(R4)
.Ldiv_b_pos:
    CMP #0, R1
    BEQ .Ldiv_zero
    CLR R2
.Ldiv_loop:
    MOV R0, R3
    SUB R1, R3
    MOV R3, R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    BIC #0xFFFE, R0
    CMP #0, R0
    BNE .Ldiv_done
    MOV R3, R0
    INC R2
    BR .Ldiv_loop
.Ldiv_done:
    MOV R2, R0
    MOV -2(R4), R3
    CMP #0, R3
    BEQ .Ldiv_end
    MOV R0, R3
    CLR R0
    SUB R3, R0
.Ldiv_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.Ldiv_zero:
    CLR R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
__mod:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #2, R6
    CLR -2(R4)
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOV R0, R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    ASR R3
    BIC #0xFFFE, R3
    CMP #0, R3
    BEQ .Lmod_a_pos
    MOV R0, R3
    CLR R0
    SUB R3, R0
    MOV #1, R3
    SUB -2(R4), R3
    MOV R3, -2(R4)
.Lmod_a_pos:
    CMP #0, R1
    BEQ .Lmod_zero
.Lmod_loop:
    MOV R0, R3
    SUB R1, R3
    MOV R3, R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    ASR R0
    BIC #0xFFFE, R0
    CMP #0, R0
    BNE .Lmod_done
    MOV R3, R0
    BR .Lmod_loop
.Lmod_done:
    MOV -2(R4), R3
    CMP #0, R3
    BEQ .Lmod_end
    MOV R0, R3
    CLR R0
    SUB R3, R0
.Lmod_end:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.Lmod_zero:
    CLR R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
