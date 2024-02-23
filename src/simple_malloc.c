#include "simple_malloc.h"


char _s_stack[];

void* simple_malloc(size_t size){
    static char* kernel_stack_head = NULL;

    if (!kernel_stack_head) {
        kernel_stack_head = _s_stack;
    }

    kernel_stack_head += size;
    return kernel_stack_head;
}