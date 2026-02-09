.ORIG 0x1000
_start:
    MOV #0xFFFE, R6
    JSR R5, main
    HALT
main:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #4, R6
    MOV R4, R0
    ADD #-4, R0
    MOV R0, R3
    SUB #4, R6
    MOV R6, R2
    MOV #4, R0
    MOV R0, -(R6)
    MOV #3, R0
    MOV R0, -(R6)
    MOV R2, R0
    MOV R0, -(R6)
    JSR R5, make
    ADD #6, R6
    MOV R6, R0
    MOV R3, R2
    MOV #4, R1
    MOV R1, -(R6)
    MOV R0, -(R6)
    MOV R2, -(R6)
    JSR R5, __memcpy
    ADD #6, R6
    ADD #4, R6
    MOV #.L.str0, R0
    MOV R0, -(R6)
    JSR R5, puts
    ADD #2, R6
    MOV R4, R0
    ADD #-4, R0
    ADD #0, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, print_int
    ADD #2, R6
    MOV #.L.str1, R0
    MOV R0, -(R6)
    JSR R5, puts
    ADD #2, R6
    MOV R4, R0
    ADD #-4, R0
    ADD #2, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, print_int
    ADD #2, R6
    MOV #10, R0
    MOV R0, -(R6)
    JSR R5, putchar
    ADD #2, R6
    MOV #0, R0
    JMP .Lreturn_main
.Lreturn_main:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
make:
    MOV R4, -(R6)
    MOV R6, R4
    SUB #4, R6
    MOV R4, R0
    ADD #-4, R0
    ADD #0, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #6, R0
    MOV (R0), R0
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV R4, R0
    ADD #-4, R0
    ADD #2, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #8, R0
    MOV (R0), R0
    MOV (R6)+, R1
    MOV R0, (R1)
    MOV 4(R4), R2
    MOV R4, R0
    ADD #-4, R0
    MOV #4, R1
    MOV R1, -(R6)
    MOV R0, -(R6)
    MOV R2, -(R6)
    JSR R5, __memcpy
    ADD #6, R6
    JMP .Lreturn_make
.Lreturn_make:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
print_int:
    MOV R4, -(R6)
    MOV R6, R4
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #0, R0
    MOV (R6)+, R1
    SUB R0, R1
    MOV R1, R0
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
    TST R0
    BNE .Lskip2
    JMP .Lelse0
.Lskip2:
    MOV #45, R0
    MOV R0, -(R6)
    JSR R5, putchar
    ADD #2, R6
    MOV R4, R0
    ADD #4, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, R1
    CLR R0
    SUB R1, R0
    MOV (R6)+, R1
    MOV R0, (R1)
    JMP .Lend0
.Lelse0:
.Lend0:
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    MOV #10, R0
    MOV (R6)+, R1
    SUB R0, R1
    MOV R1, R0
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
    MOV #1, R1
    SUB R0, R1
    MOV R1, R0
    TST R0
    BNE .Lskip5
    JMP .Lelse3
.Lskip5:
    MOV #10, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, __div
    ADD #4, R6
    MOV R0, -(R6)
    JSR R5, print_int
    ADD #2, R6
    JMP .Lend3
.Lelse3:
.Lend3:
    MOV #48, R0
    MOV R0, -(R6)
    MOV #10, R0
    MOV R0, -(R6)
    MOV R4, R0
    ADD #4, R0
    MOV (R0), R0
    MOV R0, -(R6)
    JSR R5, __mod
    ADD #4, R6
    MOV (R6)+, R1
    ADD R0, R1
    MOV R1, R0
    MOV R0, -(R6)
    JSR R5, putchar
    ADD #2, R6
.Lreturn_print_int:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.L.str1:
    .WORD 0x7320
    .WORD 0x7274
    .WORD 0x6375
    .WORD 0x5F74
    .WORD 0x6572
    .WORD 0x7574
    .WORD 0x6E72
    .WORD 0x203A
    .WORD 0x3D62
    .WORD 0x0000
.L.str0:
    .WORD 0x7473
    .WORD 0x7572
    .WORD 0x7463
    .WORD 0x725F
    .WORD 0x7465
    .WORD 0x7275
    .WORD 0x3A6E
    .WORD 0x6120
    .WORD 0x003D
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
fopen:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    MOV 6(R4), R1
    MOVB (R1), R2
    CMP #0x0072, R2
    BEQ .Lfopen_r
    CMP #0x0077, R2
    BEQ .Lfopen_w
    CMP #0x0061, R2
    BEQ .Lfopen_a
    CLR R1
    BR .Lfopen_mode
.Lfopen_r:
    CLR R1
    BR .Lfopen_chkplus
.Lfopen_w:
    MOV #1, R1
    BR .Lfopen_chkplus
.Lfopen_a:
    MOV #2, R1
.Lfopen_chkplus:
    MOV 6(R4), R2
    MOVB 1(R2), R2
    CMP #0x002B, R2
    BNE .Lfopen_mode
    MOV #3, R1
.Lfopen_mode:
    TRAP #20
    CMP #0xFFFF, R0
    BEQ .Lfopen_fail
    INC R0
    BR .Lfopen_ret
.Lfopen_fail:
    CLR R0
.Lfopen_ret:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
fread:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 6(R4), R2
    MOV 8(R4), R3
    TST R2
    BEQ .Lfread_zero
    TST R3
    BEQ .Lfread_zero
    MOV R3, -(R6)
    MOV R2, -(R6)
    JSR R5, __mul
    ADD #4, R6
    MOV R0, R2
    MOV 10(R4), R0
    TST R0
    BEQ .Lfread_zero
    DEC R0
    MOV 4(R4), R1
    TRAP #21
    MOV 6(R4), R1
    CMP #1, R1
    BEQ .Lfread_ret
    MOV R1, -(R6)
    MOV R0, -(R6)
    JSR R5, __div
    ADD #4, R6
.Lfread_ret:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.Lfread_zero:
    CLR R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
fwrite:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 6(R4), R2
    MOV 8(R4), R3
    TST R2
    BEQ .Lfwrite_zero
    TST R3
    BEQ .Lfwrite_zero
    MOV R3, -(R6)
    MOV R2, -(R6)
    JSR R5, __mul
    ADD #4, R6
    MOV R0, R2
    MOV 10(R4), R0
    TST R0
    BEQ .Lfwrite_zero
    DEC R0
    MOV 4(R4), R1
    TRAP #22
    MOV 6(R4), R1
    CMP #1, R1
    BEQ .Lfwrite_ret
    MOV R1, -(R6)
    MOV R0, -(R6)
    JSR R5, __div
    ADD #4, R6
.Lfwrite_ret:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.Lfwrite_zero:
    CLR R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
fclose:
    MOV R4, -(R6)
    MOV R6, R4
    MOV 4(R4), R0
    TST R0
    BEQ .Lfclose_bad
    DEC R0
    TRAP #23
    BR .Lfclose_ret
.Lfclose_bad:
    MOV #0xFFFF, R0
.Lfclose_ret:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
fseek:
    MOV R4, -(R6)
    MOV R6, R4
    MOV #0xFFFF, R0
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
ftell:
    MOV R4, -(R6)
    MOV R6, R4
    MOV #0xFFFF, R0
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
    MOV R3, R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    ASR R2
    BIC #0xFFFE, R2
    CMP #0, R2
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
