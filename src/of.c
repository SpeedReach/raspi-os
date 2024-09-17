#include "of.h"
#include "align.h"
#include "debug.h"
#include "endian.h"
#include "stddef.h"
#include "strings.h"
#include "mm/dynamic_allocator.h"

device_node* root = NULL;


const uint8_t* parse_scope(device_node* scope_head,const uint8_t* cur);
const uint8_t* parse_node(device_node* node,const uint8_t* cur);
const uint8_t* parse_property(property* prop,const uint8_t* cur);


void of_init(const void *dtb){
    g_blob = dtb;
    g_fdt_header = (const fdt_header*) dtb;
    g_dt_str_start = g_blob + rev_u32(g_fdt_header->off_dt_strings);
}

void of_parse_fdt(){
    const uint8_t* cur = (uint8_t*) (g_blob + rev_u32(g_fdt_header->off_dt_struct));
    root = (device_node*) kmalloc(sizeof(device_node));
    if (!root) {
        PANIC("kmalloc failed");
    }
    d_printfln("root: %x", root);
    parse_scope(root, cur);
}

void add_property(device_node* node, property* prop){
    if(node->properties == NULL){
        node->properties = prop;
    } else {
        property* tail = node->properties;
        while(tail->next != NULL){
            tail = tail->next;
        }
        tail->next = prop;
    }
}

/**
 * @brief start parsing a property
 * 
 * @param prop where to store the parsed property
 * @param cur  where to start parsing
 * @return uint8_t*  where the next property starts
 */
const uint8_t *parse_property(property *prop, const uint8_t *cur){
    ASSERT_EQ(FDT_PROP, rev_u32(*(uint32_t*) cur), "Should be start of property\n");
    cur += sizeof(uint32_t);
    const fdt_property* fdt_prop = (fdt_property*) cur;
    prop->length = rev_u32(fdt_prop->len);
    prop->name = (char*) (g_dt_str_start + rev_u32(fdt_prop->nameoff));
    cur += sizeof(fdt_property);
    prop->value = (void*) cur;
    cur += prop->length;
    cur = ALIGN_4(cur);
    return cur;
}


/**
 * @brief start parsing a node
 * 
 * @param node where to store the parsed node
 * @param cur  where to start parsing
 * @return uint8_t*  where the next node starts
 */
const uint8_t *parse_node(device_node *node, const uint8_t *cur){
    cur = eat_no_op(cur);
    ASSERT_EQ(FDT_BEGIN_NODE, rev_u32(*(uint32_t*) cur), "Should be start of node\n");
    cur += sizeof(uint32_t);
    node->name = (char*) cur;
    cur += strlen(node->name) + 1;
    cur = ALIGN_4(cur);
    //d_printfln("node begin %s", node->name);
    while (1)
    {
        const uint32_t token = rev_u32(*(uint32_t*) cur);
        switch (token){
            case FDT_BEGIN_NODE: {
                device_node* child = (device_node*) kmalloc(sizeof(device_node));
                ASSERT(node->child == NULL, "%s already have a child %s", node->name, node->child->name);
                node->child = child;
                cur = parse_scope(child, cur);
                continue;
            }
            case FDT_END_NODE: {
                cur += sizeof(uint32_t);
                goto end_parse_node;
            }
            case FDT_PROP: {
                property* prop = kmalloc(sizeof(property));
                if (prop == NULL) {
                    PANIC("Failed to kmalloc memory for property.")
                }
                add_property(node, prop);
                cur = parse_property(prop, cur);
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

end_parse_node:
    return cur;
}

const uint8_t *parse_scope(device_node *scope_head, const uint8_t *cur){
    ASSERT_EQ(FDT_BEGIN_NODE, rev_u32(*(uint32_t*) cur), "Should be start of scope\n");
    cur = parse_node(scope_head, cur);
    device_node* tail = scope_head;
    while(1){
        const int token = rev_u32(*(uint32_t*) cur);
        if(token == FDT_END_NODE || token == FDT_END){
            break;
        }
        device_node* sib = kmalloc(sizeof(device_node));
        if (sib == NULL) {
            PANIC("Failed to allocate memory for sib\n");
        }
        cur = parse_node(sib, cur);
        //printf("sibling: %s\n", sib->name);
        tail->sibling = sib;
        tail = sib;
    }
    return cur;
}


device_node* of_find_node_by_path(char* path){
    device_node* node = root;
    char* token = strtok(path, "/");
    while(token != NULL){
        //printf("token: %s\n", token);
        device_node* child = NULL;
        bool found = false;
        for_each_child_of_node(node, child){
            if(strcmp(child->name, token) == 0){
                token = strtok(NULL, "/");
                node = child;
                found = true;
                break;
            }
        }
        if (!found){
            return NULL;
        }
    }
    return node;
}

property* of_find_property(device_node* node, char* name){
    property* prop = NULL;
    for_each_property_of_node(node, prop){
        if(strcmp(prop->name, name) == 0){
            return prop;
        }
    }
    return NULL;
}


int of_scan_flat_dt(int (*it)(const uint8_t* node,
				     const char *name, int depth,
				     void *data),
			   void *data)
{
    int  rc = 0, depth = 0;
    const uint8_t* cur = (const uint8_t*) g_blob + rev_u32(g_fdt_header->off_dt_struct);
	for (cur = fdt_next_node(cur, &depth);
	     cur != NULL && depth >= 0 && !rc;
	     cur = fdt_next_node(cur, &depth)) {
		const char *pathp = fdt_get_name(cur);
		rc = it(cur, pathp, depth, data);
	}
	return rc;
}

const void* of_get_flat_dt_prop(const uint8_t* node_start, const char* prop_name, int* len){
    return fdt_get_prop(node_start, prop_name, len);
}


