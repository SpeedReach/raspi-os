#include "asm/sysregs.h"

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret

.globl put32
put32:
    str w1, [x0]
    ret

.globl get32
get32:
    ldr w0, [x0]
    ret

.globl getEl
getEl:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

.globl fromEl2ToEl1
fromEl2ToEl1:
	ldr	x1, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x1

	ldr	x1, =HCR_VALUE
	msr	hcr_el2, x1

	ldr	x1, =SPSR_VALUE
	msr	spsr_el2, x1

	msr	elr_el2, lr

	eret

