#include "regdef.h"
#include "asm.h"
#include "bios.h"

.section .bios.entry
.global _start
_start:
	j real_start
	nop
.section .bios.exception
bios_intr_handler:
	mfc0 t0, CP0_EPC
	add t0,t0,4
	mtc0 t0, CP0_ERROREPC
	add k0,sp,0
	li t0,BIOS_INTR_STACK
	add sp,t0,0
	lw k1,bios_exception_vec
	jalr k1
	add sp,k0,0
	nop
	eret
	nop

.section .text
real_start:
	/* Disable interrupts */
    mtc0	zero, CP0_STATUS
	nop
    /* Disable watch exception. */
    mtc0    zero, CP0_WATCHLO
	nop
    mtc0    zero, CP0_WATCHHI
	nop
    /* Disable kernel mode cache */
	mfc0	t0, CP0_CONFIG
	and	t0, ~0x7
	ori	t0, 0x2
	mtc0	t0, CP0_CONFIG

	/* set up stack */
	li	sp, BIOS_STACK_BASE

	la t0,intr_handler_from_kernel
	sw t0,bios_exception_vec
	/* jump to main */
	jal	bios_main
		
loop:
	j	loop
	nop

.align 4
.section .bss
.global tree
tree:
	.word 0
	.word 0

.global bios_exception_vec
bios_exception_vec:
	.word 0


.section .data
flag:
	.ascii "flag{the_second_flag_is_here}\000"
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0




