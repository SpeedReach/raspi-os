#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "initrd.h"
#include "mm/buddy.h"
#include "device_tree.h"
#include "endian.h"

void test(const void* data, const uint32_t len){
    printf("ha %x\n", rev_u32(*(uint32_t*) data));
}

void kernel_main(const void *const dtb_addr){
    init_dt(dtb_addr);
    uart_init();
    init_printf(0,uart_putc);
    printf("dtb_addr at: %x\n", dtb_addr);
    fdt_traverse("chosen","linux,initrd-start", initramfs_callback);

    init_buddy_system();

    list_files();
    
    printf("current el: %d\n", getEl());

    //asm("mov x0, 0\n\t"
    //    "add x0, x0, 1\n\t"
    //    "svc 0");

    
    //TEST_BUDDY();
    

    while (1)
    {
        /* code */
        
    }
    
}
