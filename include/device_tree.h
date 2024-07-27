#ifndef _DEVICE_TREE_H
#define _DEVICE_TREE_H


#include <stdint.h>
#include <stdbool.h>


typedef struct __attribute__((packed)) fdt_header_t {
 uint32_t magic;
 uint32_t totalsize;
 uint32_t off_dt_struct;
 uint32_t off_dt_strings;
 uint32_t off_mem_rsvmap;
 uint32_t version;
 uint32_t last_comp_version;
 uint32_t boot_cpuid_phys;
 uint32_t size_dt_strings;
 uint32_t size_dt_struct;
} fdt_header;

typedef struct __attribute__((packed)) fdt_reserve_entry_t {
 uint64_t address;
 uint64_t size;
} fdt_reserve_entry;


typedef struct __attribute__((packed)) fdt_property_t{
 uint32_t len;
 uint32_t nameoff;
} fdt_property;

//data, and data size(bytes)
typedef void (*call_back_func)(const void *, const uint32_t);


bool init_dt(const void *const dtb_addr);
void fdt_traverse(const char * node_name, const char * prop_name, call_back_func call_back);


#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

#endif