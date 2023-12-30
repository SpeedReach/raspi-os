#include "mini_uart.h"
#include "utils.h"
#include "shell/shell.h"
#include "peripherals/mailbox.h"
#include "peripherals/irq.h"
#include "printf.h"
#include "simple_allocator.h"
#include "file_system.h"


void kernel_main(void)
{
	uart_init();
	init_printf(0,uart_putc);
	irq_vector_init();
	simple_malloc_init();
	load_file_system();
	
	int el = get_el();
    printf("Exception level: %d \r\n", el);

	uart_send_string("Hello, world!\r\n");
	
	print_board_revision();
	print_arm_memory();

	
	while (1) {
		wait_command();
	}
}
