
#include "mm/buddy.h"
#include "simple_malloc.h"
#include "printf.h"
#include "debug.h"

buddy_node* free_buddy_lists[MAX_BLOCK_ORDER + 1] = {NULL};
buddy_node buddy_array[FRAME_COUNT] = {0};


#define N_FRAMES_PER_BLOCK(ORDER) (1 << ORDER)
#define IS_HEAD(NODE) (NODE->head_order >= 0)


void* 
map_phy_addr(buddy_node* node);

buddy_node* 
map_to_buddy(void* addr);

void 
insert_free_list(buddy_node* node);

buddy_node* 
alloc_free_block(size_t size);

buddy_node* 
release_redundant(buddy_node* node, size_t target_size);

void
attempt_merge_free_node(buddy_node* node);

//print the node val for nodes with idx < max_nodes
void print_state(int max_nodes);



/**
 * @brief initializes the free_buddy_lists[MAX_BLOCK_ORDER] with a max order linked list, and sets the buddy_array with correct idx.
 */
void init_buddy_system(){
    // setup the buddy array
    for(int i=0;i<FRAME_COUNT;i++){
        buddy_array[i].idx = i;
        buddy_array[i].next_free = NULL;
        buddy_array[i].last_free = NULL;
        buddy_array[i].free = 1;
        // the node value should be MAX_BLOCK_ORDER if it's the start of a max order block
        // else just FREE_FRAME_VALUE
        if((i % (1 << MAX_BLOCK_ORDER)) == 0){
            buddy_array[i].head_order = MAX_BLOCK_ORDER;
        }
        else{
            buddy_array[i].head_order = NONE_HEAD_ORDER;
        }
    }
    
    // setup the free buddy list
    for(int i=0;i<MAX_BLOCK_ORDER;i++){
        free_buddy_lists[i] = NULL;
    }
    // set the MAX_BLOCK_ORDER free_list's head to the first element in the buddy array,
    // which should also be the first block.
    free_buddy_lists[MAX_BLOCK_ORDER] = &buddy_array[0];
    
    //link the max order free_buddy_list
    for( int i=0 ; i < (TOTAL_BUDDY_SIZE/MAX_BLOCK_SIZE) - 1; i++ ){
        buddy_array[i * (MAX_BLOCK_SIZE/FRAME_SIZE)].next_free = &buddy_array[(i+1) * (MAX_BLOCK_SIZE/FRAME_SIZE)];
        buddy_array[(i+1) * (MAX_BLOCK_SIZE/FRAME_SIZE)].last_free = &buddy_array[i * (MAX_BLOCK_SIZE/FRAME_SIZE)];
    }
    //the last element's next_free should point to NULL
    //set the last element of the free_buddy_list's next_free to NULL
    buddy_array[FRAME_COUNT - (MAX_BLOCK_SIZE/FRAME_SIZE) ].next_free = NULL;

}

void* alloc_buddy(size_t size){
    buddy_node* node = alloc_free_block(size);
    if(!node){
        PANIC("NO MORE MEMORY!!!");
    }
    node = release_redundant(node, size);
    return map_phy_addr(node);
}

void free_buddy(void* addr){
    buddy_node * node = map_to_buddy(addr);
    if (node == NULL){
        return;
    }
    if(node->free){
        d_printf("This buddy is already free");
        return;
    }
    if(node->head_order < 0){
        d_printf("This buddy is not a head of a block.");
        return;
    }

    d_printf("freeing buddy %d with order %d", node->idx, node->head_order);

    node->free = 1;
    insert_free_list(node);
    attempt_merge_free_node(node);
}


void extract_from_free(buddy_node* node){
    
    if(node->head_order < 0){
        d_printf("Cannot extract a node that is not a head from the free list");
        return;
    }
    if(!node->free){
        d_printf("This node is already not free.");
        return;
    }

    if(!node->last_free){
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

void attempt_merge_free_node(buddy_node* node){
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

    d_printf("merged node %d and %d to a %d order node", left_node->idx, right_node->idx,left_node->head_order);
    d_printf("%x", left_node->next_free);
    insert_free_list(left_node);
    attempt_merge_free_node(left_node);
}




int align_size(size_t size){
    size_t tmp = size;
    
    int i = 0;
    while (tmp > FRAME_SIZE)
    {
        ++i;
        tmp >>= 1;
    }

    if(size > (1 << i) * FRAME_SIZE){
        ++i;
    }
    
    return i;
}


//calculate physial address
void* map_phy_addr(buddy_node* node){
    if(node->head_order < 0){
        d_printf("warning: should not access phy address with a not head buddy_node!");
        return NULL;
    }
    return (void*) (BUDDY_START + FRAME_SIZE * (long)(node->idx));
}



buddy_node* release_redundant(buddy_node* node, size_t target_size){
    ASSERT(IS_HEAD(node), "should not release a none head node");
    ASSERT(!node->free, "should not release an unallocated node %d", node->idx);

    const size_t target_nframes = target_size/FRAME_SIZE + ((target_size % FRAME_SIZE) ? 1 : 0);
    d_printf("releasing target_nframes %u", target_nframes);

    while (N_FRAMES_PER_BLOCK(node->head_order)/2 >= target_nframes && !node->free)
    {
        d_printf("releasing, current node order %d", node->head_order);
        //put the bottom half to the lower order free_list
        buddy_node* bottom_half_start = &buddy_array[node->idx + N_FRAMES_PER_BLOCK(node->head_order)/2];
        node->head_order --;
        bottom_half_start->head_order = node->head_order;
        insert_free_list(bottom_half_start);
    }
    d_printf("end of releasing, current node order: %u", node->head_order);
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

    //find place to insert, linear search
    while (1)
    {
        // should insert/append after cursor
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


buddy_node* alloc_free_block(size_t size) {
    int depth = align_size(size);
    d_printf("depth: %d", depth);
    while (depth <= MAX_BLOCK_ORDER)
    {
        if(!free_buddy_lists[depth]){
            d_printf("cannot find free block in depth %d", depth);
            depth ++;
            continue;
        }
        d_printf("found free block in depth %d", depth);
        buddy_node* free_block = free_buddy_lists[depth];

        free_buddy_lists[depth] = free_block->next_free;
        free_block ->next_free->last_free = NULL;
        free_block ->next_free = NULL;
        
        int n_frames = N_FRAMES_PER_BLOCK(depth);

        for(int i=0;i<n_frames;i++){
            buddy_array[free_block->idx + i].free = 0;
        }
        
        return free_block;
    }
    return NULL;
}




buddy_node* map_to_buddy(void* addr){
    intptr_t ptr =(intptr_t) addr;

    ASSERT(ptr >= BUDDY_START, "invalid mem range.");
    ASSERT(ptr < BUDDY_END, "invalid mem range");
    int idx = (ptr - BUDDY_START) / FRAME_SIZE;
    return &buddy_array[idx];
}

void print_state(int max_nodes){
#ifdef DEBUG
    printf("free list: \n");
    for(int i=0;i<MAX_BLOCK_ORDER;i++){
        if(free_buddy_lists[i]){
            printf("%d ",free_buddy_lists[i]->idx);
        }
        else{
            printf("empty ");
        }
    }
    printf("\n");
    for(int i=0;i<max_nodes;i++){
        printf("%d,%d  ", buddy_array[i].free, buddy_array[i].head_order);
    }
#endif
}

void TEST_BUDDY(){
    init_buddy_system();
    int* a1 = alloc_buddy(sizeof(int));
    ASSERT(a1, "a1 should be valid");
    *a1 = 10;
    ASSERT(!buddy_array[0].free, "first element should be allocated.");
    ASSERT(buddy_array[0].head_order == 0, "head order should be 0");
    ASSERT(buddy_array[1].free, "second element should not be allocated");
    ASSERT(*(int*)map_phy_addr(&buddy_array[0]) == 10,"a1 != 10 ???");

    int *a2 = alloc_buddy(sizeof(int));
    ASSERT(!buddy_array[1].free, "second element should be allocated");
    *a2 = 20;
    ASSERT(*(int*)map_phy_addr(&buddy_array[1]) == 20,"a2 != 20 ???");
    free_buddy(a1);

    ASSERT(buddy_array[0].free, "first element should be free.");

    print_state(16);
    int* a3 = alloc_buddy(sizeof(int));
    printf("%d", *a3);
    *a3 = 14;
    printf("%d", *a3);


    void* a5 = alloc_buddy(0x1000 *2 +1);
    print_state(16);
    free_buddy(a2);
    free_buddy(a3);
}


