#ifndef DYNAMIC_ALLOCATOR_H
#define DYNAMIC_ALLOCATOR_H

#include <stdint.h>
#include <assert.h>
#include "buddy.h"
#include <stddef.h>

typedef struct slab_type_t{
    uint8_t index;
    uint8_t slot_amount;
    uint16_t slot_size;
} __attribute__((packed)) slab_type;

typedef struct slab{
    slab_type type;
    void* memory;
    struct slab* next_free_slab;
} __attribute__((packed)) slab_t;


#define TYPES 5

static const slab_type SLAB_TYPES[TYPES] = {
    {.index = 0, .slot_amount = 32, .slot_size = 16},
    {.index = 1, .slot_amount = 16, .slot_size = 32},
    {.index = 2, .slot_amount = 8, .slot_size = 64},
    {.index = 3, .slot_amount = 4, .slot_size = 128},
    {.index = 4, .slot_amount = 2, .slot_size = 256}
};

#define AVAILABLE_BYTES (512 * 5)

typedef struct page{
    struct page* next_allocated_page;
    struct page* last_allocated_page;
    //make it easier to check if the page is all unallocated.
    uint8_t remain_slots[TYPES];
    struct slab* free_lists[TYPES];
    struct slab slabs[32+16+8+4+2];
    char memory[AVAILABLE_BYTES];
}  __attribute__((__packed__)) page_t;

static_assert(sizeof(page_t) < FRAME_SIZE);

void init_dynamic_allocator();
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
