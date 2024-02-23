#ifndef _PANIC_H
#define _PANIC_H


#include "printf.h"


#define DEBUG





#define PANIC(...)  \
    printf("\n%s:%d: panic: \n",  __FILE__, __LINE__);    \
    printf(__VA_ARGS__);    \
    printf("\n");    \
    while(1);


#ifdef DEBUG
#define ASSERT(VALUE, ...) \
    if(!(VALUE)) {    \
        printf("\n%s:%d: assert: \n",  __FILE__, __LINE__);  \
        printf(__VA_ARGS__);    \
        while(1);     \
    }
#define d_printf(...) printf("%s:%d: debug:   ",__FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")

#else
#define ASSERT(VALUE, ...) \
    if(!(VALUE)) {    \
        printf(__VA_ARGS__);    \
    } 

#define d_printf(...) ;

#endif


#endif