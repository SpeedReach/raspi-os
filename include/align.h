#ifndef ALIGN_H

#define ALIGN_H

#include<stdint.h>

#define ALIGN_UP_4(ptr) ((uintptr_t)(ptr) % 4 == 0 ? (void *)((uintptr_t)(ptr) + 4) : (void *)(((uintptr_t)(ptr) + 3) & ~3))

#define ALIGN_4(ptr) ((void *)(((uintptr_t)(ptr) + 3) & ~3))


#endif