#ifndef OF_H
#define OF_H

#include <stddef.h>
#include "device_tree.h"
#include "endian.h"


typedef struct device_node_t {
	const char *name;
	struct	property_t *properties;
	struct	device_node_t *parent;
	struct	device_node_t *child;
	struct	device_node_t *sibling;
} device_node;


typedef struct property_t {
	char	*name;
	int	length;
	void	*value;
	struct property_t *next;
} property;



extern device_node *root;


void of_init(const void *dtb);

/**
 * @brief This function is used to scan the flattened device-tree, it is
 * used to extract the memory information at boot before we can
 * unflatten the tree.
 * 
 * @param it: callback function
 * @param data: context data pointer
 */
int of_scan_flat_dt(int (*it)(const uint8_t* node,
				     const char *name, int depth,
				     void *data),
			   void *data);


const void* of_get_flat_dt_prop(const uint8_t* node_start, const char* prop_name, int* len);
#define for_each_dt_child_of_node(depth, node, child_start) \
	int tmp_depth = depth; \
	for(child_start = fdt_next_node(node, &tmp_depth); \
		tmp_depth == depth + 1; \
		child_start = fdt_next_node(child_start, &tmp_depth))

/**
 * @brief parse the flat device tree and build the tree structure
 *  must be called after of_init
 */
void of_parse_fdt();

device_node* of_find_node_by_path(char* path);
property* of_find_property(device_node* node, char* name);



#define for_each_node(node) \
	for (node = root; node != NULL; node = node->sibling)

#define for_each_child_of_node(node, child_n) \
	for(child_n = node->child;child_n != NULL; child_n = child->sibling)

#define for_each_property_of_node(node, prop) \
	for(prop = node->properties; prop != NULL; prop = prop->next)


#endif

