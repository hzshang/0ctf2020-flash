#include "regdef.h"
#include "asm.h"
#include "bios.h"

.section .entry
.global _start
_start:
    # init timer
    jal reload_timer
    # enable timer interupt, enter kernel mode
    la t0,0x10008001
    # la t0,0x10000000
    mtc0 t0,CP0_STATUS
    la sp, KERNEL_STACK_BASE
    j kernel_main
    nop


.global goto_bios
goto_bios:
    mfc0 t0, CP0_STATUS
    addiu sp, sp, -8
    sw t0, (sp)
    sw ra, 4(sp)
#    or t0, t0, 0x400000 # 01001
#    and t0,t0,~0xff
    li t0,0x00400004
    mtc0 t0, CP0_STATUS
    syscall
    nop

    lw t0, (sp)
    lw ra, 4(sp)
    mfc0 t0, CP0_STATUS
    addiu sp, sp, 8
    jr ra
    nop

.global reload_timer
reload_timer:
    la t0, TIMER_INTR
    mtc0 t0, CP0_COMPARE
    mtc0 zero, CP0_COUNT
    jr ra
    nop

.section .exception
except_handler:
    .set noat
    addu k1, zero, sp
    li sp, KERNEL_INTR_STACK
    addiu sp, sp, -148
    sw AT, (sp)
    sw v0, 4(sp)
    sw v1, 8(sp)
    sw a0, 12(sp)
    sw a1, 16(sp)
    sw a2, 20(sp)
    sw a3, 24(sp)
    sw t0, 28(sp)
    sw t1, 32(sp)
    sw t2, 36(sp)
    sw t3, 40(sp)
    sw t4, 44(sp)
    sw t5, 48(sp)
    sw t6, 52(sp)
    sw t7, 56(sp)
    sw s0, 60(sp)
    sw s1, 64(sp)
    sw s2, 68(sp)
    sw s3, 72(sp)
    sw s4, 76(sp)
    sw s5, 80(sp)
    sw s6, 84(sp)
    sw s7, 88(sp)
    sw t8, 92(sp)
    sw t9, 96(sp)
    sw gp, 108(sp)
    sw k1, 112(sp)
    sw fp, 116(sp)
    sw ra, 120(sp)
    mfc0 t0, CP0_EPC
    sw t0,124(sp)
    mfc0 t0, CP0_ERROREPC
    sw t0, 128(sp)
    mfc0 t0, CP0_STATUS
    sw t0, 132(sp)
    add a0,sp,0
    lw k0,exception_vec
    jalr k0
    nop
    lw t0, 124(sp)
    mtc0 t0, CP0_EPC
    lw t0, 128(sp)
    mtc0 t0,CP0_ERROREPC
    lw t0, 132(sp)
    mtc0 t0,CP0_STATUS
    lw v0, 4(sp)
    lw v1, 8(sp)
    lw a0, 12(sp)
    lw a1, 16(sp)
    lw a2, 20(sp)
    lw a3, 24(sp)
    lw t0, 28(sp)
    lw t1, 32(sp)
    lw t2, 36(sp)
    lw t3, 40(sp)
    lw t4, 44(sp)
    lw t5, 48(sp)
    lw t6, 52(sp)
    lw t7, 56(sp)
    lw s0, 60(sp)
    lw s1, 64(sp)
    lw s2, 68(sp)
    lw s3, 72(sp)
    lw s4, 76(sp)
    lw s5, 80(sp)
    lw s6, 84(sp)
    lw s7, 88(sp)
    lw t8, 92(sp)
    lw t9, 96(sp)
    lw gp, 108(sp)
    lw k1, 112(sp)
    lw fp, 116(sp)
    lw ra, 120(sp)
    add sp, k1,zero
    eret
    nop

.section .data
.align 4
.global exception_vec
exception_vec:
    .word intr_handler

.global vm
vm:
    .word 0        # regs[0][1]
    .word 0        # mul
    .word 0        # pc
    .word base_ptr # base
    .word VM_SP_TOP # top
    .word base_ptr # sp
    .word 0        # in



.section .bss
.global base_ptr
base_ptr:
    .space 0x1000




# vm code
# push 0
# push reg[0]
# cmp 
# jmp do_multi if true
#

