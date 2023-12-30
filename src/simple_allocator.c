

extern void* const heap_begin;
extern void* const heap_end;

void* usedHeap;

void simple_malloc_init(){
    usedHeap = heap_begin + 4;
}

void* simple_malloc(unsigned int size){
    void* tmp = usedHeap;
    usedHeap += size;
    return tmp;
}


