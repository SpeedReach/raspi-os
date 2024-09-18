

.section ".text.boot"

.globl _start
_start:
    mrs     x1, mpidr_el1
	and	x1, x1,#0xFF		// Check processor id
	cbz	x1, master		// Hang for all non-primary CPU
	b	proc_hang

proc_hang:
    b   proc_hang

master:
    bl fromEl2ToEl1
    adr     x1, _s_bss
    adr     x2, _e_bss
    bl memzero
    ldr x1, =_s_stack    // Load stack ptr
    mov sp, x1
    bl set_exception_vector_table
    bl kernel_main
    b  proc_hang

memzero:
    str     xzr, [x1]
    add     x1, x1, #8
    cmp     x1, x2
    bne     memzero
    ret



.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm


.macro interrupt_handler handler_func
.align 7
save_all
bl   \handler_func
load_all
eret
.endm


.align 11 // vector table should be aligned to 0x800
vector_table:
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler irq_router
  interrupt_handler default_exception_handler

  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler

  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler

  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler
  interrupt_handler default_exception_handler


set_exception_vector_table:
  adr x1, vector_table
  msr vbar_el1, x1
  ret
