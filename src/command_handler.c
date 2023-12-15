#include "mini_uart.h"
#include "printf.h"
#include "commands/commands.h"
#include "strcmp.h"

void wait_command( void){
    const static int maxCommandLength = 12;
    char cmd[13];
    int currentIndex = 0;
    while(1){
        char c = uart_recv();
        uart_send(c);
        if (c == '\r' || c == -1){
            uart_send('\n');
            if(currentIndex < maxCommandLength){
                cmd [currentIndex] = '\0';
            }
            break;
        }
        if(currentIndex >= maxCommandLength){
            continue;
        }
        cmd[currentIndex] = c;
        currentIndex ++;
    }
    runCommand(cmd);
}

void runCommand(const char* command){
    if(strCmp(command,"help") == 0){
        help();
    }
    else if(strCmp(command,"hello") == 0){
        hello_world();
    }
    else if(strCmp(command, "board_rev") == 0){
        board_rev();
    }
}