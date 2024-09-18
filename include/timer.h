#ifndef TIMER_H
#define TIMER_H

#define CORE_IRQ_SOURCE 0x40000060

void core_timer_el0_init(void);
void core_timer_el0_enable(void);
void core_timer_el0_disable(void);

#endif