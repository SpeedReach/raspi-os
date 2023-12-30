#include "mini_uart.h"
#include "printf.h"
#include "shell/commands.h"
#include "strings.h"


#define MAX_CMD_LEN 100


void wait_command( void){
    uart_send_string("$ ");
    char buffer[MAX_CMD_LEN];

    int index = 0;
    while (1)
    {
        const char c = uart_recv();
        if(c == '\n' || c == '\r'){
            uart_send('\n');
            buffer[index] = '\0';
            break;
        }
        if( c == '\x7f') { //backspace
            if(index > 0){
                uart_send('\b'); //Go back one char and replace with space and go back
                uart_send(' ');
                uart_send('\b');
                index --;
            }  
            continue;  
        }

        if(index < MAX_CMD_LEN){
            uart_send(c);
            buffer[index] = c;
            index ++;
        }
    }
    runCommand(buffer);
}



void runCommand(const char* command){
    if(strcmp(command,"help") == 0){
        help();
    }
    else if(strcmp(command,"hello") == 0){
        hello_world();
    }
    else if(strcmp(command, "reboot") == 0){
        reboot();
    }
    else if (strcmp(command, "ls") == 0)
    {
        list_files();
    }
    else{
        printf("Unknown command\n");
    }
    
}