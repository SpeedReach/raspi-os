#include "mm/memblock.h"
#include "of.h"
#include "printf.h"
#include "debug.h"
#include "endian.h"
#include "strings.h"
#include "utils.h"


struct memblock_region init_mem_regions[10];
struct memblock_region init_reserved_regions[10];

struct memblock g_memblock =  {
    .memory = {
        .cnt = 0,
        .regions = init_mem_regions,
        .max = 10,
        .total_size = 0
    },
    .reserved = {
        .cnt = 0,
        .max = 10,
        .regions = init_reserved_regions,
        .total_size = 0
    }
};


int early_init_dt_scan_memory(const uint8_t* node,
				     const char *name, int depth,
				     void *data){
    if(strcmp(name ,"memory@0") != 0){
        return 0;
    }
    int len;
    const uint32_t* reg = of_get_flat_dt_prop(node, "reg", &len);
    ASSERT(reg != NULL, "reg is null");
    for (int i = 0; i < len/sizeof(uint32_t); i+=2){
        const phys_addr_t base = rev_u32(reg[i]);
        const phys_addr_t size = rev_u32(reg[i+1]);
        d_printfln("adding memory@0 base: %x, size: %x", base, size);
        memblock_add(base, size);
    }
    return 1;
}

int early_init_dt_scan_reserved(const uint8_t* node,
                     const char *name, int depth,
                     void *data){
    if(strcmp(name ,"reserved-memory") != 0){
        return 0;
    }
    const uint8_t* child_start;
    for_each_dt_child_of_node(depth, node, child_start){
        int len;
        const uint32_t* reg = of_get_flat_dt_prop(child_start, "reg", &len);
        if(reg == NULL){
            continue;
        }
        for (int i = 0; i < len/sizeof(uint32_t); i+=2){
            const phys_addr_t base = reg[i];
            const phys_addr_t size = reg[i+1];
            d_printfln("reserved-memory base: %x, size: %x for %s", base, size, fdt_get_name(child_start));
            memblock_reserve(base, size);
        }
    }
    return 1;
}

int early_init_dt_scan_initramfs(const uint8_t* node,
                     const char *name, int depth,
                     void *data){
    if(strcmp(name ,"chosen") != 0){
        return 0;
    }
    const uint32_t* start = of_get_flat_dt_prop(node, "linux,initrd-start", NULL);
    ASSERT(start != NULL, "start is null");
    const uint32_t* end = of_get_flat_dt_prop(node, "linux,initrd-end", NULL);
    ASSERT(end != NULL, "end is null");
    const uint32_t start_val = rev_u32(*start);
    const uint32_t end_val = rev_u32(*end);
    phys_addr_t base = start_val;
    phys_addr_t size = end_val - start_val;
    d_printfln("Reserving initrd base: %x, size: %x", base, size);
    memblock_reserve(base, size);
    return 1;
}


void debuggg(){
    d_printfln("\n");
    d_printfln("Memory regions");
    for(int i=0;i<g_memblock.memory.cnt;i++){
        d_printfln("base: %x, size: %x", g_memblock.memory.regions[i].base, g_memblock.memory.regions[i].size);
    }
    d_printfln("Reserved regions");
    for(int i=0;i<g_memblock.reserved.cnt;i++){
        d_printfln("base: %x, size: %x", g_memblock.reserved.regions[i].base, g_memblock.reserved.regions[i].size);
    }
    d_printfln("Total memory size: %x", g_memblock.memory.total_size);
    d_printfln("Total reserved size: %x", g_memblock.reserved.total_size);
    d_printfln("\n");
}


extern char* _s_text;
extern char* _s_heap;
void reserve_kernel(){
    phys_addr_t base = (phys_addr_t) &_s_text;
    phys_addr_t end = (phys_addr_t) &_s_heap;
    phys_addr_t size = end - base;
    d_printfln("Reserving kernel base: %x, size: %x, end: %x", base, size, end);
    memblock_reserve(base, size);
}


#define SPIN_TABLE_BASE 0x0000
#define SPIN_TABLE_SIZE 0X1000
void reserve_spin_tables(){
    d_printfln("Reserving spin table base: %x, size: %x", SPIN_TABLE_BASE, SPIN_TABLE_SIZE);
    memblock_reserve(SPIN_TABLE_BASE, SPIN_TABLE_SIZE);
}

void memblock_init(void){
    of_scan_flat_dt(early_init_dt_scan_memory, NULL);
    of_scan_flat_dt(early_init_dt_scan_reserved, NULL);
    of_scan_flat_dt(early_init_dt_scan_initramfs, NULL);
    reserve_spin_tables();
    reserve_kernel();
}

int rgn_cmp(const void* a, const void* b){
    struct memblock_region* rgn1 = (struct memblock_region*) a;
    struct memblock_region* rgn2 = (struct memblock_region*) b;
    if(rgn1->base < rgn2->base){
        return -1;
    }
    if(rgn1->base > rgn2->base){
        return 1;
    }
    return 0;
}

void insert_region(struct memblock_type* type, struct memblock_region rgn){
    ASSERT(type->cnt < type->max, "Region array is full");
    struct memblock_region* region;
    if(type == &g_memblock.memory){
        region = &init_mem_regions[type->cnt];
    } else if(type == &g_memblock.reserved){
        region = &init_reserved_regions[type->cnt];
    } else{
        UNREACHABLE();
    }
    *region = rgn;
    type->total_size += rgn.size;
    type->cnt++;
    quickSort(type->regions, 0, type->cnt-1, sizeof(struct memblock_region), rgn_cmp);
}

void merge_overlapping_regions(struct memblock_type* type, struct memblock_region new_rgn){
    // new_rgn may overlap with multiple regions
    // we need to merge all overlapping regions
    int i;
    bool merged = false;
    // find the first overlapping region and merge new_rgn into it.
    phys_addr_t merged_start;
    phys_addr_t merged_end;
    for(i=0;i<type->cnt;i++){
        struct memblock_region* rgn = &type->regions[i];
        if(new_rgn.base + new_rgn.size < rgn->base){
            continue;
        }
        if(new_rgn.base > rgn->base + rgn->size){
            continue;
        }
        if (new_rgn.base == rgn->base && new_rgn.size == rgn->size){
            return;
        }
        merged = true;
        merged_start = MIN(new_rgn.base, rgn->base);
        merged_end = MAX(new_rgn.base + new_rgn.size, rgn->base + rgn->size);
        break;
    }
    // if new_rgn doesn't overlap with any other region, insert it
    if(!merged){
        insert_region(type, new_rgn);
        return;
    }
    //find first overlap from right to left
    int j = type->cnt - 1;
    for(;j>i;j--){
        if(type->regions[j].base < merged_end){
            printf("a\n");
            merged_end = MAX(merged_end, type->regions[j].base + type->regions[j].size);
            break;
        }
    }
    //shift unmerged regions to the left
    for(int k=j+1;k<type->cnt;k++){
        printf("b\n");
        type->regions[k-(j-i)] = type->regions[k];
    }
    type->cnt -= j-i;
    type->regions[i].base = merged_start;
    type->regions[i].size = merged_end - merged_start;

    //recalculate total size
    type->total_size = 0;
    for(int k=0;k<type->cnt;k++){
        type->total_size += type->regions[k].size;
    }
}

void memblock_add(phys_addr_t base, phys_addr_t size){
    struct memblock_region rgn = {
        .base = base,
        .size = size
    };
    merge_overlapping_regions(&g_memblock.memory, rgn);
}


void memblock_reserve(phys_addr_t base, phys_addr_t size){
    struct memblock_region rgn = {
        .base = base,
        .size = size
    };
    merge_overlapping_regions(&g_memblock.reserved, rgn);
}

// if base is already in a free zone, we return the next free zone
// if base is in a reserved zone, we return the next free zone after the reserved zone
phys_addr_t find_next_free_zone(phys_addr_t base ,phys_addr_t* size){
    struct memblock_region* free_rgn = NULL;
    for(int i=0;i<g_memblock.memory.cnt;i++){
        struct memblock_region* rgn = &g_memblock.memory.regions[i];
        if(rgn->base <= base && base < rgn->base + rgn->size){
            free_rgn = rgn;
            break;
        }
    }

    if(free_rgn == NULL){
        d_printfln("base: %x is not in any available memory", base);
        *size = 0;
        return INVALID_PA;
    }
    
    //check if base is in a reserved zone, if so, return the next free zone
    for(int i=0;i<g_memblock.reserved.cnt;i++){
        struct memblock_region* rgn = &g_memblock.reserved.regions[i];
        if(rgn->base <= base && base < rgn->base + rgn->size){
            base = rgn->base + rgn->size;
            if(i == g_memblock.reserved.cnt - 1){
                *size = free_rgn->base + free_rgn->size - base;
            }
            else{
                *size = g_memblock.reserved.regions[i+1].base - base;
            }
            return base;
        }
    }

    //base is in a free zone
    for(int i=0;i<g_memblock.reserved.cnt;i++){
        struct memblock_region rgn = g_memblock.reserved.regions[i];
        if(rgn.base > base){
            printf("xxxxx %d base: %x, size: %x\n", i, rgn.base, rgn.size);
            base = rgn.base + rgn.size;
            if(i == g_memblock.reserved.cnt - 1){
                *size = free_rgn->base + free_rgn->size - base;
            }
            else{
                *size = g_memblock.reserved.regions[i+1].base - base;
            }
            return base;
        }
    }
    *size =0;
    return INVALID_PA;
}

void* memblock_alloc(phys_addr_t size, phys_addr_t align){
    phys_addr_t base = 0;
    phys_addr_t rgn_size;
    for(base = find_next_free_zone(base, &rgn_size);base != INVALID_PA && size != 0; base = find_next_free_zone(base, &rgn_size)){
        if(rgn_size < size){
            continue;
        }
        if(align != 0){
            base = ALIGN(base, align);
        }
        if(rgn_size < size){
            continue;
        }
        d_printfln("allocating base: %x, size: %x", base, size);
        memblock_reserve(base, size);
        return (void*) base;
    }
    return NULL;
}

phys_addr_t get_total_memory_size(){
    return g_memblock.memory.total_size;
}