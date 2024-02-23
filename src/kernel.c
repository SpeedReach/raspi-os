#include "utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "initrd.h"
#include "mm/buddy.h"



void kernel_main(void){
    uart_init();
    init_printf(0,uart_putc);
    
    init_buddy_system();
    printf("current el: %d\n", getEl());

    asm("mov x0, 0\n\t"
        "add x0, x0, 1\n\t"
        "svc 0");

    list_files();
    TEST_BUDDY();

    while (1)
    {
        /* code */
    }
    
}
