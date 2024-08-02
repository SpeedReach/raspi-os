#include "mm/memblock.h"
#include "of.h"
#include "printf.h"
#include "debug.h"
#include "endian.h"
#include "strings.h"

memblock g_memblock =  {
    .current_limit = 128
};

int early_init_dt_scan_memory(const uint8_t* node,
				     const char *name, int depth,
				     void *data){
    if(strcmp(name ,"memory@0") != 0){
        return;
    }
    uint32_t* reg = of_get_flat_dt_prop(node, "reg");
    ASSERT(reg != NULL, "reg is null");
    phys_addr_t base = rev_u32(reg);
    phys_addr_t size = rev_u32(reg + 4);
    printf("base: %x size: %x\n", base, size);
}


void memblock_init(void){
    of_scan_flat_dt(early_init_dt_scan_memory, NULL);
}

void memblock_add(phys_addr_t base, phys_addr_t size){

}
int memblock_reserve(phys_addr_t base, phys_addr_t size);
void * memblock_alloc(phys_addr_t size, phys_addr_t align);
void memblock_free(phys_addr_t base, phys_addr_t size);

