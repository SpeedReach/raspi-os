#ifndef	_BOOT_H
#define	_BOOT_H

#include "stdint.h"

#define ALIGN(x, a) (((x)+(a)-1)&~((a)-1))


extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern int getEl();


#endif  /*_BOOT_H */
