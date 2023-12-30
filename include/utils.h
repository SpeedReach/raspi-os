#ifndef	_BOOT_H
#define	_BOOT_H

#include "stdint.h"

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern int get_el();

uint32_t hex2u32_8(char *buf);
#endif  /*_BOOT_H */
