#include<stddef.h>
#include "strings.h"
#include "align.h"
#include "debug.h"
#include "utils.h"
#include "device_tree.h"
#include "printf.h"
#include "endian.h"

const fdt_header* g_fdt_header = NULL;
const char* g_blob = NULL; 
const char* g_dt_str_start = NULL;

const uint8_t* eat_no_op(const uint8_t* cur){
    while (rev_u32(*(uint32_t*) cur) == FDT_NOP)
    {
        cur += sizeof(uint32_t);
    }
    return cur;
}


const uint8_t* eat_prop(const uint8_t* cur){
    cur += sizeof(uint32_t);
    fdt_property* prop = (fdt_property*) cur;
    cur += sizeof(fdt_property);
    cur += rev_u32(prop->len);
    cur = ALIGN_4(cur);
    return cur;
}

//eat the first token and the name of the node
const uint8_t* eat_node_start(const uint8_t* cur){
    ASSERT_EQ(FDT_BEGIN_NODE, rev_u32(*(uint32_t*) cur), "Should be start of node\n");
    cur += sizeof(uint32_t);
    cur += strlen((char*)cur) + 1;
    cur = ALIGN_4(cur);
    return cur;
}

const uint8_t* fdt_next_node(const uint8_t* cur, int *depth)
{
    cur = eat_node_start(cur);
    uint32_t token;
    while (1)
    {
        token = rev_u32(*(uint32_t*) cur);
        switch (token){
            case FDT_BEGIN_NODE: {
                (*depth)++;
                return cur;
            }
            case FDT_END_NODE: {
                (*depth)--;
                cur += sizeof(uint32_t);
                if(*depth < 0){
                    return cur;
                }
                continue;
            }
            case FDT_PROP: {
                cur = eat_prop(cur);
                continue;
            }
            case FDT_NOP: {
                cur += sizeof(uint32_t);
                continue;
            }
            default: {
                PANIC("Unknown token: %d\n", token);
            }
        }
    }
    return cur;
}

const char *fdt_get_name(const uint8_t* cur){
    return (const char *) cur + sizeof(uint32_t);
}


const void* fdt_get_prop(const uint8_t* node_start, const char* prop_name, int* len){
    const uint8_t* cur = eat_node_start(node_start);
    while (1)
    {
        const uint32_t token = rev_u32(*(uint32_t *) cur);
       if (token == FDT_END_NODE || token == FDT_END || token == FDT_BEGIN_NODE){
            break;
       }
       ASSERT_EQ(FDT_PROP, token, "Should be a property\n");
       cur += sizeof(uint32_t);
        const fdt_property *prop = (fdt_property *) cur;
       cur += sizeof(fdt_property);
       if(strcmp(prop_name, g_dt_str_start + rev_u32(prop->nameoff)) == 0){
            *len = rev_u32(prop->len);
            return cur;
       }
       cur += rev_u32(prop->len);
       cur = ALIGN_4(cur);
    }
    return NULL;
}

