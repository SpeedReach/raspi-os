
#include <exceptions.h>


void enable_irq(void){
    asm volatile("msr    daifclr, #2");
}

void disable_irq(void){
  asm volatile("msr    daifset, #2");
}