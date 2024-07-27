
#include<device_tree.h>
#include<printf.h>
#include<endian.h>
#include<stddef.h>
#include "strings.h"
#include "align.h"
#include "debug.h"


static const char *_dtb_start = NULL;
static bool success = 0;

bool init_dt(const void *const dtb_addr) {
    _dtb_start = dtb_addr;
    success = rev_u32(((const fdt_header*)_dtb_start)->magic) == 0xd00dfeed;
    
    return success;
}



void fdt_traverse(const char* filter_node_name, const char* filter_prop_name, call_back_func call_back) {
    const fdt_header* header = (const fdt_header*) _dtb_start;
    const char* const dt_str_start = _dtb_start + rev_u32(header->off_dt_strings);
    uint8_t* cur = (uint8_t*) (_dtb_start + rev_u32(header->off_dt_struct));
    
    while(1){
        uint32_t token = rev_u32(*(uint32_t*) cur);
        if (token != FDT_BEGIN_NODE){
            break;
        }
        cur += sizeof(uint32_t);
        char node_name[50];
        int copied = strcpy(node_name, (char*) cur, 100);
        bool node_matched = strcmp(node_name, filter_node_name) == 0;
        //printf("node name: %s\n", node_name);
        cur += copied;
        cur = ALIGN_4(cur);
        while(1){
            token = rev_u32(*(uint32_t*) cur);
            if(token == FDT_BEGIN_NODE){
                break;
            }
            switch(token){
                case FDT_END_NODE: {
                    cur += sizeof(uint32_t);
                    break;
                }
                case FDT_PROP: {
                    cur += sizeof(uint32_t);
                    fdt_property* prop = (fdt_property*) cur;
                    uint32_t len = rev_u32(prop->len);
                    uint32_t nameoff = rev_u32(prop->nameoff);
                    cur += sizeof(fdt_property);
                    //printf("property name: %s\n", dt_str_start + nameoff);
                    if (node_matched && strcmp((char*) (dt_str_start + nameoff), filter_prop_name) == 0){
                        call_back(cur, prop->len);
                    }
                    cur += len;
                    cur = ALIGN_4(cur);
                    break;
                }
                case FDT_NOP: {
                    cur += sizeof(uint32_t);
                    break;
                }
                case FDT_END: {
                    goto end_traverse;
                }
                default: {
                    PANIC("unknown %d\n", token);
                }
            }
        }
    }

end_traverse:

}