#include "mm/dynamic_allocator.h"
#include "mm/buddy.h"
#include <assert.h>



page* allocated_pages = NULL;

// alloc from the buddy system and append to the allocated_pages list
page* internal_alloc(){
    page* p = (page*) alloc_buddy(1);
    
    if(p == NULL){
        d_printf("Failed to alloc mem from buddy system.");
        return NULL;
    }

    //initialization
    int slab_count = 0;
    void* used_memory = &(p->memory);
    for(int i=0;i<TYPES;i++){
        slab_type type = SLAB_TYPES[i];
        p->remain_slots[i] = type.slot_amount;
        for(int j=0;j<type.slot_size;j++){
            p->slabs[slab_count].type = type;
            p->slabs[slab_count].memory = used_memory;
            p->slabs->next_free_slab = &p->slabs[slab_count+1];
            slab_count++;
            used_memory += type.slot_size;
        }
        p->slabs[slab_count-1].next_free_slab = NULL;
    }

    //insert into the front of the allocated pages
    p->last_allocated_page = NULL;
    p->next_allocated_page = allocated_pages;
    allocated_pages = p;
}

//check whether a page is okay to free.
int should_free(page* p){
    for(int i=0;i<TYPES;i++){
        if(p->remain_slots[i] != SLAB_TYPES[i].slot_amount){
            return 0;
        }
    }
    return 1;
}


void init_dynamic_allocator(){
    init_buddy_system();
}

//find the nearest and larger slab for the size
//size should not be bigger than the largest slab size
slab_type round_up(size_t size){
    for(int i=0;i<TYPES;i++){
        if(SLAB_TYPES[i].slot_size >= size ){
            return SLAB_TYPES[i];
        }
    }
    slab_type invalid = {
        .slot_amount = 0,
        .slot_size = 0
    };
    return invalid;
}

void* kmalloc(size_t size){
    if(allocated_pages == NULL){
        allocated_pages = internal_alloc();
    }
    if(size > SLAB_TYPES[TYPES -1].slot_size){
        //allocated a memory larger than a page
        return alloc_buddy(size);
    }
    
    slab_type type = round_up(size);
    page* p = allocated_pages;
    
    //find an allocated page with an available slab with the target type.
    int has_free = 0;
    while (1)
    {
        if(p->free_lists[type.index]){
            has_free = 1;
            break;
        }
    }

    if(!has_free){
        p = internal_alloc();
        if(p == NULL){
            d_printf("cannot allocate memory for target size %u", size);
            return NULL;
        }
    }

    //remove slab from free list and return it's corresponding memory.
    slab* return_slab = &(p->free_lists[type.index]);
    p->free_lists[type.index] = return_slab->next_free_slab;
    return_slab->next_free_slab = NULL;
    return return_slab->memory;
}



void kfree(void* ptr){
    page* p = allocated_pages;
    while (p)
    {
        if(ptr >= p->memory && ptr < (p->memory + AVAILABLE_BYTES)){
            break;
        }
        p = p->next_allocated_page;
    }

    if(p == NULL){
        d_printf("this memory is not allocated from this dynamic allocator.");
        return;
    }
    
    void* cursor = &p->memory;
    int slab_index = 0;
    for(int i=0;i<TYPES;i++){
        size_t size_for_type = SLAB_TYPES[i].slot_amount * SLAB_TYPES[i].slot_size;
        if(cursor  <= ptr && ptr < p->memory + size_for_type){
            d_printf("memory belongs to slab type %d with size %u", SLAB_TYPES[i].index, SLAB_TYPES[i].slot_size);
            
            slab_index += (ptr - cursor) / SLAB_TYPES[i].slot_size;
            slab* slab = &p->slabs[slab_index];

            slab->next_free_slab = p->free_lists[i];
            p->free_lists[i] = slab;
            break;
        }

        cursor += size_for_type;
        slab_index += SLAB_TYPES[i].slot_amount;
    }

    if(should_free(p)){
        if(p->last_allocated_page){
            page* last_page = p->last_allocated_page;
            last_page->next_allocated_page = p->next_allocated_page;
            p->next_allocated_page->last_allocated_page = last_page;
        }
        else{
            allocated_pages = p->next_allocated_page;
            p->next_allocated_page->last_allocated_page = NULL;
        }
        p->next_allocated_page = NULL;
        p->last_allocated_page = NULL;
        free_buddy(p);
    }
}
