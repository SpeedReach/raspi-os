#include <exceptions.h>
#include <stdio.h>
#include <timer.h>

#include "printf.h"
#include "utils.h"
#include "peripherals/irq.h"


void core_timer_handler();

void default_exception_handler(){
    uint64_t spsr_el1, elr_el1, esr_el1;
    asm volatile (
        "mrs %0, spsr_el1\n\t"   // Move the current physical count into cntpct
        "mrs %1, elr_el1\n\t"   // Move the counter frequency into cntfrq
        "mrs %2, esr_el1\n\t"
        : "=r" (spsr_el1), "=r" (elr_el1), "=r" (esr_el1)  // Output operands
        :                           // No input operands
        :                           // No clobbers
    );
    printf("values: %x %x %x\n", spsr_el1, elr_el1, esr_el1);
}

void irq_router() {

    //put32(DISABLE_BASIC_IRQS, 1);
    const int src = get32(CORE_IRQ_SOURCE);
    printf("%d", src);
    switch (src) {
        case 2:
            core_timer_handler();
        break;
        default: ;
    }
    //put32(ENABLE_BASIC_IRQS, 1);
}

void core_timer_handler() {

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
    uint64_t core_timer_freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(core_timer_freq));
    core_timer_freq &= 0xffffffff;

    // Print the number of seconds since boot.

    uint64_t timer_val;
    asm volatile("mrs %0, cntpct_el0" : "=r"(timer_val));

    printf("%lu seconds since boot freq %lu\n", timer_val / core_timer_freq, core_timer_freq);

    // Set the time to interrupt to 2 seconds later.

    asm volatile("msr cntp_tval_el0, %0" : : "r"(2 * core_timer_freq));
    //asm volatile ("msr DAIFClr, %0" : : "i" (0xF));
    return;
}