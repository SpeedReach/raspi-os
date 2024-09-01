#include <types.h>


struct memblock_region {
    phys_addr_t base;  /* Start address of the region */
    phys_addr_t size;  /* Size of the region */
};


struct memblock_type {
    unsigned long cnt;              /* Number of regions */
    unsigned long max;              /* Maximum number of regions */
    struct memblock_region *regions; /* Array of memory regions */
    phys_addr_t total_size;         /* Total size of all regions */
};


struct memblock {
    struct memblock_type memory;  /* Available memory regions */
    struct memblock_type reserved; /* Reserved memory regions */
};


extern struct memblock g_memblock;


void memblock_init(void);
void memblock_add(phys_addr_t base, phys_addr_t size);
void memblock_reserve(phys_addr_t base, phys_addr_t size);
void * memblock_alloc(phys_addr_t size, phys_addr_t align);


// size is a out* parameter
phys_addr_t find_next_free_zone(phys_addr_t base ,phys_addr_t* size);
phys_addr_t get_total_memory_size();