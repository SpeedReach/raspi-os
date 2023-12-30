#ifndef IRQ_H
#define IRQ_H

#include "base.h"

#define IRQ_BASIC_PENDING ( PBASE + 0xB200)

void irq_vector_init(void);
void enable_irq(void);
void disable_irq(void);

#endif