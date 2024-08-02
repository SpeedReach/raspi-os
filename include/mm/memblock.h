#include <types.h>


typedef struct memblock_region_t {
    phys_addr_t base;  /* Start address of the region */
    phys_addr_t size;  /* Size of the region */
} memblock_region;


typedef struct memblock_type_t {
    unsigned long cnt;              /* Number of regions */
    unsigned long max;              /* Maximum number of regions */
    memblock_region *regions; /* Array of memory regions */
    phys_addr_t total_size;         /* Total size of all regions */
} memblock_type;

typedef struct memblock_t {
    memblock_type memory;  /* Available memory regions */
    memblock_type reserved; /* Reserved memory regions */
} memblock;


extern memblock g_memblock;

void memblock_init(void);
void memblock_add(phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);
void * memblock_alloc(phys_addr_t size, phys_addr_t align);
void memblock_free(phys_addr_t base, phys_addr_t size);