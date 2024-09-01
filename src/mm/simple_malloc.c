#include "mm/simple_malloc.h"
#include "strings.h"

char _s_heap[];

void* simple_malloc(size_t size) {
    d_printfln("heap head: %x", _s_heap);
    static char* kernel_heap_head = NULL;

    if (!kernel_heap_head) {
        kernel_heap_head = _s_heap;
    }

    // Decrement the stack pointer by the requested size
    kernel_heap_head -= size;

    // Align the stack pointer to the nearest 32-byte boundary
    uintptr_t aligned_address = ((uintptr_t)kernel_heap_head) & ~(uintptr_t)(31);
    kernel_heap_head = (char*)aligned_address;

    // Zero out the allocated memory
    memset(kernel_heap_head, 0, size);

    return kernel_heap_head;
}

