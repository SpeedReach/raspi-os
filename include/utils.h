#ifndef	_BOOT_H
#define	_BOOT_H

#include <stdint.h>
#include <stddef.h>

#define ALIGN(x, a) (((x)+(a)-1)&~((a)-1))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );
extern int getEl();

void quickSort(void* base, int low, int high, size_t size, int (*cmp)(const void*, const void*));


#endif  /*_BOOT_H */
