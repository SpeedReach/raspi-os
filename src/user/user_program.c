
#include <exceptions.h>
#include <timer.h>
#include <user/user_program.h>
#include <mm/dynamic_allocator.h>




void run(uintptr_t program){
    void* user_stack = kmalloc(4196);

    asm volatile("msr spsr_el1, %0" : : "r"(0x3C0));

    asm volatile (

        // Set ELR_EL1 to the program's start address
        "msr elr_el1, %0\n"             // Move program_start to ELR_EL1

        // Set SP_EL0 to the user's stack pointer
        "msr sp_el0, %1\n"              // Move user_stack to SP_EL0

        // Issue ERET to return to EL0 and start executing user code
        "eret\n"
        :
        : "r"(program), "r"(user_stack) // Input operands
        : "x2"                                // Clobbered registers
    );

    kfree(user_stack);
}