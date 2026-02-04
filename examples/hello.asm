.ORIG 0x1000
_start:
    MOV #0xFFFE, R6
    JSR R5, main
    HALT
main:
    MOV R4, -(R6)
    MOV R6, R4
    MOV #.L.str0, R0
    MOV R0, -(R6)
    JSR R5, puts
    ADD #2, R6
    MOV #0, R0
    JMP .Lreturn_main
.Lreturn_main:
    MOV R4, R6
    MOV (R6)+, R4
    RTS R5
.L.str0:
    .WORD 0x6568
    .WORD 0x6C6C
    .WORD 0x3A6F
    .WORD 0x4820
    .WORD 0x6C65
    .WORD 0x6F6C
    .WORD 0x202C
    .WORD 0x4450
    .WORD 0x2D50
    .WORD 0x3131
    .WORD 0x0A21
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
