#ifndef _SIMPLE_MALLOC_H
#define _SIMPLE_MALLOC_H

#include <stddef.h>

extern char _s_stack[];

void* simple_malloc(size_t size);

#endif