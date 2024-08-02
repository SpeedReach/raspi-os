#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "initrd.h"
#include "mm/buddy.h"
#include "mm/dynamic_allocator.h"
#include "mm/memblock.h"
#include "device_tree.h"
#include "endian.h"
#include "of.h"



void kernel_main(const void *const dtb_addr){
    uart_init();
    init_printf(0,uart_putc);

    run_tests();

    printf("dtb_addr at: %x\n", dtb_addr);
    of_init(dtb_addr);
    memblock_init();
    
    of_parse_fdt();
    initramfs();
    
    
    init_dynamic_allocator();
    list_files();
    TEST_BUDDY();
    test_slab_index_mapping();
    test_dynamic_allocator();

    device_node* node = of_find_node_by_path("reserved-memory");
    ASSERT(node != NULL, "node is null");
    printf("node: %s\n", node->name);
    property* prop;
    for_each_property_of_node(node, prop){
        printf("prop: %s %d\n", prop->name, prop->length);
    }
    
    device_node* child;
    for_each_child_of_node(node, child){
        printf("child: %s\n", child->name);
        for_each_property_of_node(child, prop){
            printf("child prop: %s %d\n", prop->name, prop->length);
        }
        device_node* child_child;
        for_each_child_of_node(child, child_child){
            printf("child child node: %s\n", child_child->name);
        }
    }

    //TEST_BUDDY();
    
    printf("current el: %d\n", getEl());
    while (1)
    {
        /* code */
        
    }
    
}
