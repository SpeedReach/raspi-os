#include "mm/buddy.h"
#include "mm/memblock.h"
#include "printf.h"
#include "debug.h"
#include "types.h"

buddy_node* free_buddy_lists[MAX_BLOCK_ORDER + 1] = {NULL};
buddy_node* buddy_array;
phys_addr_t buddy_base;
phys_addr_t buddy_total_size;

#define N_FRAMES_PER_BLOCK(ORDER) (1 << ORDER)
#define IS_HEAD(NODE) (NODE->head_order >= 0)

enum lr {
    LEFT,
    RIGHT
};

uintptr_t 
map_phy_addr(buddy_node* node);

int
map_to_buddy_index(uintptr_t addr);

buddy_node* 
map_to_buddy(void* addr);

void 
insert_free_list(buddy_node* node);

buddy_node* 
alloc_free_block(size_t size);

buddy_node* 
release_redundant(buddy_node* node, size_t target_size, enum lr release_direction);

void
attempt_merge_free_node(buddy_node* node);

void init_free_head(buddy_node* node, int order){
    //first find the order of the block
    node->head_order = order;
    node->free = 1;
    node->next_free = NULL;
    node->last_free = NULL;
    for (int i = 1; i < N_FRAMES_PER_BLOCK(order); i++)
    {
        buddy_node* tmp = node + i;
        tmp->head_order = NONE_HEAD_ORDER;
        tmp->free = 1;
    }
    insert_free_list(node);
}

int find_ideal_max_order(int buddy_idx ,phys_addr_t size){
    int ideal_order = MAX_BLOCK_ORDER;
    while (1)
    {
        if(buddy_idx % N_FRAMES_PER_BLOCK(ideal_order) == 0){
            break;
        }
        ideal_order --;
    }
    return ideal_order;
}

int find_optimal_order(phys_addr_t max_size, int max_order){
    int order = max_order;
    while (order >= 0)
    {
        if(N_FRAMES_PER_BLOCK(order) * FRAME_SIZE >= max_size){
            order --;
            continue;
        }
        break;
    }
    return order;
}

void init_free_zone(phys_addr_t base, size_t size){
    d_printfln("init free zone %x %x", base, size);
    phys_addr_t zone_end = base + size; //exclusive
    int buddy_idx = map_to_buddy_index(base);
    int end_idx = map_to_buddy_index(zone_end);
    d_printfln("start idx: %d, end idx: %d", buddy_idx, end_idx);
    while (1)
    {
        int order = find_ideal_max_order(buddy_idx, size);
        //d_printfln("ideal order for %d size %x is %d", buddy_idx, size, order);
        order = find_optimal_order(size, order);
        //d_printfln("optimal is %d", order);
        if(order < 0){
            break;
        }
        init_free_head(&buddy_array[buddy_idx], order);
        buddy_idx += N_FRAMES_PER_BLOCK(order);
        size -= N_FRAMES_PER_BLOCK(order) * FRAME_SIZE;
    }
}

/**
 * @brief initializes the free_buddy_lists[MAX_BLOCK_ORDER] with a max order linked list, and sets the buddy_array with correct idx.
 */
void init_buddy_system(phys_addr_t base, phys_addr_t size){
    d_printfln("init buddy system\n");
    buddy_base = base;
    buddy_total_size = size;
    int buddy_count = size/FRAME_SIZE;
    buddy_array = (buddy_node*) memblock_alloc(buddy_count * sizeof(buddy_node), FRAME_SIZE);
    d_printfln("buddy_count: %x", buddy_count);
    d_printfln("buddy_array: %x", buddy_array);
    d_printfln("buddy_base: %x", buddy_base);
    d_printfln("buddy_total_size: %x", buddy_total_size);

    for(int i=0;i<buddy_count;i++){
        buddy_array[i].idx = i;
        buddy_array[i].free = 0;
        buddy_array[i].head_order = 0;
        buddy_array[i].next_free = NULL;
        buddy_array[i].last_free = NULL;
    }

    d_printfln("buddy_array initialized");

    phys_addr_t zone_size;
    base = find_next_free_zone(base, &zone_size);
    for(; base != INVALID_PA; base = find_next_free_zone(base, &zone_size)){
        //d_printfln("zone_base: %x", base);
        //d_printfln("zone_size: %x", zone_size);
        if(zone_size < FRAME_SIZE){
            //d_printfln("zone size is smaller than frame size, skip");
            continue;
        }
        init_free_zone(base, zone_size);
    }
}




void* buddy_system_alloc(size_t size){
    buddy_node* node = alloc_free_block(size);
    if(!node){
        PANIC("NO MORE MEMORY!!!");
    }
    node = release_redundant(node, size, RIGHT);
    return map_phy_addr(node);
}

void buddy_system_free(void* addr){
    buddy_node * node = map_to_buddy(addr);
    ASSERT(node, "invalid address");
    ASSERT(!node->free, "should not free an already free block");
    ASSERT(node->head_order >= 0, "should not free a none head block");

    d_printfln("freeing buddy %d with order %d", node->idx, node->head_order);
    node->free = 1;
    insert_free_list(node);
    attempt_merge_free_node(node);
}


void extract_from_free(buddy_node* node){
    printf("extracting %d %x %x\n", node->idx, node->last_free, node->next_free);
    ASSERT(node->head_order >= 0, "Cannot extract a node that is not a head from the free list")
    ASSERT(node->free, "This node is already not free.")

    if(node->last_free == NULL){
        free_buddy_lists[node->head_order] = node->next_free;
        if(node->next_free){
            node->next_free->last_free = NULL;
        }
    }
    else{
        node->last_free->next_free = node->next_free;
        if(node->next_free){
            node->next_free->last_free = node->last_free;
        }
    }

    node->last_free = NULL;
    node->next_free = NULL;
}


//Big problem here, the buddy_node is not correctly merged. 
void attempt_merge_free_node(buddy_node* node){
    d_printfln("attempt merging %d", node->idx);
    if(node->head_order == MAX_BLOCK_ORDER){
        return;
    }
    buddy_node* left_node;
    buddy_node* right_node;

    int is_right_block = node->idx % (2*N_FRAMES_PER_BLOCK(node->head_order));
    if (is_right_block){
        left_node = &buddy_array[node->idx - N_FRAMES_PER_BLOCK(node->head_order)];
        right_node = node;
        if(!left_node->free){
            return;
        }
    }
    else{
        right_node = &buddy_array[node->idx + N_FRAMES_PER_BLOCK(node->head_order)];
        left_node = node;
        if(!right_node->free){
            return;
        }
    }


    extract_from_free(left_node);
    extract_from_free(right_node);

    
    
    right_node->head_order = -1;
    left_node->head_order ++;

    d_printfln("merged node %d and %d to a %d order node", left_node->idx, right_node->idx,left_node->head_order);
    d_printfln("%x", left_node->next_free);
    insert_free_list(left_node);
    attempt_merge_free_node(left_node);
}



//align the size to the nearest 2^n * FRAME_SIZE, returns n
int align_size(size_t size){
    ASSERT(size < buddy_total_size, "size should be smaller than the total buddy size");
    size_t tmp = FRAME_SIZE;
    int i = 0;
    while(tmp < size){
        tmp <<= 1;
        i++;
    }
    return i;
}


//calculate physial address
uintptr_t map_phy_addr(buddy_node* node){
    ASSERT(node->head_order >= 0, "should not access phy address with a not head buddy_node!")
    return (buddy_base + FRAME_SIZE * (long)(node->idx));
}



buddy_node* release_redundant(buddy_node* node, size_t target_size, enum lr release_direction){
    ASSERT(IS_HEAD(node), "should not release a none head node");
    ASSERT(!node->free, "should not release an unallocated node %d", node->idx);

    const size_t target_nframes = target_size/FRAME_SIZE + ((target_size % FRAME_SIZE) ? 1 : 0);
    d_printfln("releasing target_nframes %u", target_nframes);

    while (N_FRAMES_PER_BLOCK(node->head_order)/2 >= target_nframes && !node->free)
    {
        d_printfln("releasing, current node order %d", node->head_order);
        if(release_direction == RIGHT){
            //put the bottom half to the lower order free_list
            buddy_node* bottom_half_start = &buddy_array[node->idx + N_FRAMES_PER_BLOCK(node->head_order)/2];
            node->head_order --;
            bottom_half_start->head_order = node->head_order;
            insert_free_list(bottom_half_start);
        }
        else{
            //put the top half to the lower order free_list, and put the bottom half to the current list.
            buddy_node* bottom_half_start = &buddy_array[node->idx + N_FRAMES_PER_BLOCK(node->head_order)/2];
            node->head_order --;
            bottom_half_start->head_order = node->head_order;
            insert_free_list(node);
            node = bottom_half_start;
        }
    }
    d_printfln("end of releasing, current node order: %u", node->head_order);
    return node;
}


void insert_free_list(buddy_node* node){
    ASSERT(IS_HEAD(node), "should not insert a none head node");
    ASSERT(!node->next_free, "node %d is already in the free list, or it is bug.", node->idx)
    ASSERT(!node->last_free, "node %d is already in the free list, or it is bug.", node->idx)

    const size_t nframes_per_block = N_FRAMES_PER_BLOCK(node->head_order);
    for(int i=0;i<nframes_per_block;i++){
        buddy_array[node->idx + i].free = 1;
    }

    //if the free_list is empty, simply assign it to the head
    buddy_node* cursor = free_buddy_lists[node->head_order];
    if(cursor == NULL){
        free_buddy_lists[node->head_order] = node;
        return;
    }
    //if the first element in free list is larger than the node we want to insert.
    if(cursor->idx + nframes_per_block > node->idx){
        free_buddy_lists[node->head_order] = node;
        node->next_free = cursor;
        cursor->last_free = node;
        return;
    }

    //find place to insert, linear search
    while (1)
    {
        if(cursor->next_free == NULL || (cursor ->idx + nframes_per_block > node->idx)){
            //insert into linked list
            buddy_node* tmp = cursor->next_free;
            cursor->next_free = node;
            node->next_free = tmp;
            tmp->last_free = node;
            break;
        }
        cursor = cursor->next_free;
    }
}

void set_block_allocated(buddy_node* block){
    if(free_buddy_lists[block->head_order] == block){
        free_buddy_lists[block->head_order] = block->next_free;
    }
    if(block->next_free != NULL){
        block->next_free->last_free = NULL;
        block->next_free = NULL;
    }
    int n_frames = N_FRAMES_PER_BLOCK(block->head_order);
    for(int i=0;i<n_frames;i++){
        buddy_array[block->idx + i].free = 0;
    }
}

buddy_node* alloc_free_block(size_t size) {
    int depth = align_size(size);
    d_printfln("depth: %d", depth);
    while (depth <= MAX_BLOCK_ORDER)
    {
        if(!free_buddy_lists[depth]){
            d_printfln("cannot find free block in depth %d", depth);
            depth ++;
            continue;
        }
        buddy_node* free_block = free_buddy_lists[depth];
        d_printfln("found free block in depth %d %d", depth, free_block->idx);
        ASSERT(free_block->head_order == depth, "");
        set_block_allocated(free_block);
        return free_block;
    }
    return NULL;
}



int map_to_buddy_index(uintptr_t addr){
    ASSERT(addr >= buddy_base, "invalid mem range %x.", addr);
    ASSERT(addr <= buddy_base + buddy_total_size, "invalid mem range %x", addr);
    return (addr - buddy_base) / FRAME_SIZE;
}

buddy_node* map_to_buddy(void* addr){
    return &buddy_array[map_to_buddy_index((uintptr_t) addr)];
}

void print_repeat(const char const* c, int amount){
    for(int i=0;i<amount;i++)
        printf("%s", c);
}
void print_space(int amount){
    print_repeat(" ", amount);
}
#define pow2(p) (1 << p)

void print_state(int id_l,int id_r){
#ifdef DEBUG
    int i;
    for(i=MAX_BLOCK_ORDER; i >= 0;i--){
        printf("i: %d,", i);
        int idx = id_l;
        buddy_node* head;
        for(head = free_buddy_lists[i];head != NULL; head = head->next_free){
            if(head->idx >= id_l && head->idx < id_r){
                if(head->free){
                    if(idx == id_l){
                        print_space(head->idx - idx);
                    }
                    else{
                        print_space(head->idx - idx - pow2(i));
                    }
                    printf("*");
                    print_repeat("-", pow2(i) - 1);
                    idx = head->idx;
                }
            }
        }
        printf("\n");
    }
#endif
}


void TEST_BUDDY(){
    printf("initial state\n");
    print_state(0, 128);
    printf("allocating a1 int\n");
    int* a1 = buddy_system_alloc(sizeof(int));
    ASSERT(a1, "a1 should be valid");
    *a1 = 10;
    print_state(0, 128);

    printf("allocating a2 int\n");
    int *a2 = buddy_system_alloc(sizeof(int));
    print_state(0, 128);
    *a2 = 20;
    printf("freeing a1 (int)\n");
    buddy_system_free(a1);

    print_state(0, 128);
    printf("allocating a3 int\n");
    int* a3 = buddy_system_alloc(sizeof(int));
    *a3 = 14;
    printf("%d", *a3);

    print_state(0, 128);
    printf("allocating a5 3 FRAME\n");
    void* a5 = buddy_system_alloc(FRAME_SIZE *2 +1);
    print_state(0, 128);
    buddy_system_free(a2);
    printf("freeing a2 (int)\n");
    print_state(0, 128);
    buddy_system_free(a3);
    printf("freeing a3 (int)\n");
    print_state(0, 128);
    buddy_system_free(a5);
    printf("freeing a5 (3 FRAME)\n");
    print_state(0, 128);
}


