
#include<user_program.h>



void run(uintptr_t program){
    //set elr to program start
    asm volatile (
        "msr elr_el1, %0\n\t"   // Set ELR_EL1 to the program address
        :
        : "r" (program)
        : "memory"
    );

    asm volatile (
        "mov x1, 0x3c5\n\t"     // Example value for SPSR_EL1
        "msr spsr_el1, x1\n\t"
        :
        :
        : "x1"
    );

    
}