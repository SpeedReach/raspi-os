#ifndef DYNAMIC_ALLOCATOR_H
#define DYNAMIC_ALLOCATOR_H

#include <stdint.h>
#include <assert.h>
#include "buddy.h"
#include <stddef.h>
#include <stdbool.h>
#include "bitops.h"

typedef struct slab_type_t{
    uint8_t index;
    uint8_t slot_amount;
    uint16_t slot_size;
} __attribute__((packed)) slab_type;


#define TYPES 5

static const slab_type SLAB_TYPES[TYPES] = {
    {.index = 0, .slot_amount = 32, .slot_size = 16},
    {.index = 1, .slot_amount = 16, .slot_size = 32},
    {.index = 2, .slot_amount = 8, .slot_size = 64},
    {.index = 3, .slot_amount = 4, .slot_size = 128},
    {.index = 4, .slot_amount = 2, .slot_size = 256}
};


#define AVAILABLE_BYTES (512 * 5)

typedef struct page_t {
    struct page_t* next_allocated_page;
    struct page_t* prev_allocated_page;
    //determine the status of each slab, 0 means free, 1 means allocated, 1 bit for each slab.
    uint8_t slabs_status[NUM_BYTES(32+16+8+4+2)];
    
    char memory[AVAILABLE_BYTES];
}  __attribute__((__packed__)) page;


static_assert(sizeof(struct page_t) <= FRAME_SIZE, "Page size exceeds frame size");

void init_dynamic_allocator();
void* kmalloc(size_t size);
void kfree(void* ptr);




void test_slab_index_mapping();
void test_dynamic_allocator();

#endif
