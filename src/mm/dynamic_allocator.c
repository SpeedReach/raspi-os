#include "mm/dynamic_allocator.h"
#include "mm/buddy.h"
#include <assert.h>
#include <stdbool.h>
#include "bitops.h"



page* allocated_pages_head = NULL;
page* allocated_pages_tail = NULL;

// alloc 1 page from the buddy system and append to the allocated_pages list
page* internal_alloc(){
    page* p = (page*) buddy_system_alloc(1);
    
    if(p == NULL){
        d_printfln("Failed to alloc mem from buddy system.");
        return NULL;
    }

    //initialization
    if (allocated_pages_head == NULL){
        allocated_pages_head = p;
        allocated_pages_tail = p;
        p->prev_allocated_page = NULL;
        p->next_allocated_page = NULL;
    }
    else{
        allocated_pages_tail->next_allocated_page = p;
        p->prev_allocated_page = allocated_pages_tail;
        allocated_pages_tail = p;
        p->next_allocated_page = NULL;
    }
    
    //set all slabs to free
    for(int i=0;i<sizeof(p->slabs_status)/sizeof(uint8_t);i++){
        p->slabs_status[i] = 0;
    }
    d_printfln("Allocated a page at %x, usable memory starts at %x \n", p, p->memory);
    return p;
}

//check whether a page is okay to free.
bool should_free(page* p){
    for(int i=0;i<sizeof(p->slabs_status)/sizeof(uint8_t);i++){
        if(p->slabs_status[i] != 0){
            return false;
        }
    }
    return true;
}

void print_page_status(page* p){
    for(int i=0;i<sizeof(p->slabs_status)/sizeof(uint8_t);i++){
        uint8_t a[1] = {p->slabs_status[i]};
        for (int i=0;i<8;i++){
            printf("%d", CHECK_BIT(a, i));
        }
        printf("\n");
    }
}

void init_dynamic_allocator(){
    init_buddy_system(0, get_total_memory_size());
}

// get start offset in memory for a slab type
int slab_type_index_offset(int type_index){
    int offset = 0;
    for(int i=0;i<type_index;i++){
        offset += SLAB_TYPES[i].slot_amount;
    }
    return offset;
}

// get the memory offset for a slab index
int slab_mem_offset(int slab_index){
    int index_type = 0;
    int a = 0;
    int offset = 0;
    for (int i=0;i<TYPES;i++){
        if(slab_index < a + SLAB_TYPES[i].slot_amount){
            index_type = i;
            break;
        }
        a += SLAB_TYPES[i].slot_amount;
        offset += SLAB_TYPES[i].slot_amount * SLAB_TYPES[i].slot_size;
    }
    offset += (slab_index - a) * SLAB_TYPES[index_type].slot_size;
    return offset;
}

// get the slab index for a memory offset
int mem_slab_index(uintptr_t mem_offset){
    int index_type = 0;
    int a = 0;
    int offset = 0;
    for (int i=0;i<TYPES;i++){
        if(mem_offset < offset + SLAB_TYPES[i].slot_amount * SLAB_TYPES[i].slot_size){
            index_type = i;
            break;
        }
        offset += SLAB_TYPES[i].slot_amount * SLAB_TYPES[i].slot_size;
        a += SLAB_TYPES[i].slot_amount;
    }
    return a + (mem_offset - offset) / SLAB_TYPES[index_type].slot_size;
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
    //d_printfln("Allocating memory of size %u", size);
    if(size > SLAB_TYPES[TYPES -1].slot_size){
        //allocated a memory larger than a page
        return buddy_system_alloc(size);
    }
    
    bool has_free = false;
    page* p = allocated_pages_head;
    if(p == NULL){
        p = internal_alloc();
        has_free = true;
    }
    slab_type type = round_up(size);
    //find an allocated page with an available slab with the target type.
    int s_status_index = 0;
    while (p && !has_free)
    {
        //get slab type index offset
        s_status_index = slab_type_index_offset(type.index);
        
        //check if the page has a free slab with the target type
        for(int i=0;i<SLAB_TYPES[type.index].slot_amount;i++){
            if(CHECK_BIT(p->slabs_status, s_status_index) == 0){
                has_free = true;
                //d_printfln("slab %d is free in %x!!!", s_status_index, p);
                break;
            }
            s_status_index ++;
        }
        if(!has_free){
            p = p->next_allocated_page;
        }
    }

    if(!has_free){
        d_printfln("No available slab with type %d allocating a new page", type.index);
        p = internal_alloc();
        if(p == NULL){
            d_printfln("cannot allocate memory for target size %u", size);
            return NULL;
        }
        s_status_index = slab_type_index_offset(type.index);
    }

    //set the slab to allocated]
    //d_printfln("s_status_index: %d", s_status_index);
    SET_BIT(p->slabs_status, s_status_index);
    ASSERT(CHECK_BIT(p->slabs_status, s_status_index), "set bit failed");
    return slab_mem_offset(s_status_index) + p->memory;
}



void kfree(void* ptr){
    page* p = allocated_pages_head;
    while (p)
    {
        if(ptr >= (void*) p->memory && ptr < ((void*) p->memory + AVAILABLE_BYTES)){
            break;
        }
        p = p->next_allocated_page;
    }

    if(p == NULL){
        d_printfln("this memory is not allocated from this dynamic allocator.");
        return;
    }
    int slab_index = mem_slab_index((uintptr_t) ptr - (uintptr_t) p->memory);
    CLEAR_BIT(p->slabs_status, slab_index);
    if(should_free(p)){
        if(p->prev_allocated_page){
            p->prev_allocated_page->next_allocated_page = p->next_allocated_page;
        }
        if(p->next_allocated_page){
            p->next_allocated_page->prev_allocated_page = p->prev_allocated_page;
        }
        if(p == allocated_pages_head){
            allocated_pages_head = p->next_allocated_page;
        }
        if(p == allocated_pages_tail){
            allocated_pages_tail = p->prev_allocated_page;
        }
        buddy_system_free(p);
    }
}

void test_slab_index_mapping(){
    d_printfln("Testing slab index mapping.");
    ASSERT_EQ(0, mem_slab_index(0), "mem_slab_index failed");
    ASSERT_EQ(2, mem_slab_index(32), "mem_slab_index failed");
    ASSERT_EQ(1, mem_slab_index(16), "mem_slab_index failed");
    ASSERT_EQ(32, mem_slab_index(512), "mem_slab_index failed");
    ASSERT_EQ(33, mem_slab_index(512 + 32), "mem_slab_index failed");
    ASSERT_EQ(0, slab_mem_offset(0), "slab_mem_offset failed");
    ASSERT_EQ(16, slab_mem_offset(1), "slab_mem_offset failed");
    ASSERT_EQ(512, slab_mem_offset(32), "slab_mem_offset failed");
    ASSERT_EQ(512 + 32,slab_mem_offset(mem_slab_index(512 + 32)), "");
    ASSERT_EQ(1024+64,slab_mem_offset(mem_slab_index(1024 + 64)), "");

    ASSERT_EQ(0, slab_type_index_offset(0), "slab_type_index_offset failed");
    ASSERT_EQ(32, slab_type_index_offset(1), "slab_type_index_offset failed");
    ASSERT_EQ(48, slab_type_index_offset(2), "slab_type_index_offset failed");
    ASSERT_EQ(56, slab_type_index_offset(3), "slab_type_index_offset failed");
    ASSERT_EQ(60, slab_type_index_offset(4), "slab_type_index_offset failed");

    d_printfln("Slab index mapping test passed.");
}


void test_dynamic_allocator() {
    printf("Testing dynamic memory allocator.\n");
    // Allocate memory of different sizes
    uint16_t*  ptr1 = (uint16_t*)  kmalloc(16);
    uint16_t* ptr1_1 = (uint16_t*) kmalloc(16);
    void* ptr2 = kmalloc(32);
    void* ptr3 = kmalloc(64);
    void* ptr4 = kmalloc(128);
    void* ptr5 = kmalloc(256);

    

    // Check if the allocations were successful
    ASSERT(ptr1 != NULL, "Allocation of 16 bytes failed.\n");
    *ptr1 = 123;
    ASSERT(ptr1_1 != NULL, "Allocation of 16 bytes failed.\n");
    *ptr1_1 = 456;

    ASSERT(ptr2 != NULL, "Allocation of 32 bytes failed.\n");
    ASSERT(ptr3 != NULL, "Allocation of 64 bytes failed.\n");
    ASSERT(ptr4 != NULL, "Allocation of 128 bytes failed.\n");
    ASSERT(ptr5 != NULL, "Allocation of 256 bytes failed.\n");

    
    ASSERT_EQ(123, *ptr1, "Memory corruption in 16 bytes allocation.\n");
    ASSERT_EQ(456, *ptr1_1, "Memory corruption in 16 bytes allocation.\n");

    // Free the allocated memory
    kfree(ptr1);
    kfree(ptr1_1);
    d_printfln("freeing ptr2\n");
    kfree(ptr2);
    d_printfln("freeing ptr3\n");
    kfree(ptr3);
    d_printfln("freeing ptr4\n");
    kfree(ptr4);
    d_printfln("freeing ptr5\n");
    kfree(ptr5);

    ASSERT(should_free((page*) ((uint8_t*)ptr1 - offsetof(page, memory))), "Should be able to free the page.\n");
    d_printfln("Deallocated all allocated memory.\n");
    ASSERT(allocated_pages_head == NULL && allocated_pages_tail == NULL, "All pages should be deallocated.\n");
    printf("Dynamic memory allocator test passed.\n");
}