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
#include "timer.h"
#include "user/user_program.h"
#include "exceptions.h"

void test_user() {
    printf("Hello World!\n");
    while(1) {
        asm volatile("svc 0");
        //printf("hiii\n");
    }
}
void kernel_main(const void *const dtb_addr){
    uart_init();
    init_printf(0,uart_putc);


    run_tests();

    printf("dtb_addr at: %x\n", dtb_addr);
    of_init(dtb_addr);
    memblock_init();

    
    init_dynamic_allocator();
    TEST_BUDDY();
    test_slab_index_mapping();
    test_dynamic_allocator();
    test_dynamic_allocator();
    of_parse_fdt();
    initramfs();
    list_files();



    const device_node* node = of_find_node_by_path("reserved-memory");
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
    

    asm volatile ("msr DAIFClr, %0" : : "i" (0xF));
    core_timer_el0_enable();
    core_timer_el0_init();
    //core_timer_el0_enable();
    printf("current el: %d\n", getEl());

    while (1)
    {
        /* code */
    }
    
}
