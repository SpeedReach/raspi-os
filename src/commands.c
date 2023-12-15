#include "mini_uart.h"


void hello_world(){
    uart_send_string("Hello world!\n");
}

#include "printf.h"

void help(){
    printf("#help\n");
    printf("help   : print this menu\n");
    printf("hello  : print hello world!\n");
    printf("reboot : reboot the device\n");
}

void board_rev(){
}
