#include "mini_uart.h"
#include "utils.h"
#include "commands/command_handler.h"

#include "printf.h"

void kernel_main(void)
{
	uart_init();
	init_printf(0,uart_putc);


	uart_send_string("Hello, world!\r\n");
	int el = get_el();
    printf("Exception level: %d \r\n", el);
	while (1) {
		wait_command();
	}
}
