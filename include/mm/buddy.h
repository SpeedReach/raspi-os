#ifndef _BUDDY_H
#define _BUDDY_H

#include <stddef.h>
#include <stdint.h>

#include "debug.h"
#include "types.h"

#define NONE_HEAD_ORDER 15

#define FRAME_SIZE 0x1000

#define MAX_BLOCK_SIZE 0x100000 //max block size contains 256 frames
// the max order should be log(MAX_BLOCK_SIZE/FRAME_SIZE) which means there are MAX_BLOCK_ORDER+1 free buddy lists
#define MAX_BLOCK_ORDER 8


typedef struct buddy_node_t
{
    uint8_t free: 1; //0 means allocated, 1 means free
    uint8_t head_order: 4; // 0 ~ MAX_BLOCK_ORDER, is -1 if it belongs to a larger block.
    uint32_t idx: 26;
    struct buddy_node_t *next_free;
    struct buddy_node_t *last_free;
}buddy_node;

extern buddy_node* free_buddy_lists[MAX_BLOCK_ORDER + 1];
extern buddy_node* buddy_array;
extern phys_addr_t buddy_base;
extern phys_addr_t buddy_total_size;


void buddy_init(phys_addr_t base, phys_addr_t size);

void* buddy_system_alloc(size_t size);

void buddy_system_free(void* addr);


void print_state(int id_l,int id_r);


void memory_reserve(uintptr_t start,uintptr_t end);

void TEST_BUDDY();

#endif