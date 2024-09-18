#include "timer.h"
#include <stdint.h>
#include "utils.h"
#include "printf.h"

#define CORE0_TIMER_IRQ_CTRL 0x40000040


void core_timer_el0_init(){
    uint64_t core_timer_freq;
    //asm volatile("mrs %0, cntfrq_el0" : "=r"(core_timer_freq));
    //asm volatile("msr cntp_tval_el0, %0" : : "r"(1 * core_timer_freq)); // Set timer for 2 * frequency
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(0x1));  // Enable timer

    uint64_t cntp_tval_el0, cntp_cval_el0,  cntpct_el0;
    asm volatile (
        "mrs %0, cntp_tval_el0\n\t"
        "mrs %1, cntp_cval_el0\n\t"
        "mrs %2, cntpct_el0\n\t"
        : "=r" (cntp_tval_el0),  "=r" (cntp_cval_el0),  "=r" (cntpct_el0)
        :                           // No input operands
        :
    );

    printf("%lu, %lu, %lu\n", cntp_tval_el0, cntp_cval_el0, cntpct_el0);
}


#define CORE0_TIMER_IRQ_CTRL 0x40000040


void core_timer_el0_enable(void) {
    asm volatile(
        "mov x0, #2\n\t"
        "ldr x1, =%0\n\t"
        "str w0, [x1]\n\t"
        : // no output operands
        : "i"(CORE0_TIMER_IRQ_CTRL)  // input operand
        : "x0", "x1"  // clobbered registers
    );
    return;
}


void core_timer_el0_disable() {
    asm volatile(
      "mov x0, #0\n\t"
      "ldr x1, =%0\n\t"
      "str w0, [x1]\n\t"
      : // no output operands
      : "i"(CORE0_TIMER_IRQ_CTRL)  // input operand
      : "x0", "x1"  // clobbered registers
    );
   return;
}

