#ifndef _PANIC_H
#define _PANIC_H


#include "printf.h"








#define PANIC(...)  \
    printf("\n%s:%d: panic: \n",  __FILE__, __LINE__);    \
    printf(__VA_ARGS__);    \
    printf("\n");    \
    while(1);

#define UNREACHABLE() PANIC("unreachable code reached")


#ifdef DEBUG
#define ASSERT(VALUE, ...) \
    if(!(VALUE)) {    \
        printf("\n%s:%d: assert: \n",  __FILE__, __LINE__);  \
        printf(__VA_ARGS__);    \
        while(1);     \
    }
#define d_printfln(...) printf("%s:%d: debug:   ",__FILE__, __LINE__); printf(__VA_ARGS__); printf("\n")
#define ASSERT_EQ(EXPECT, ACTUAL, ...) \
    { \
    int __a = ACTUAL; \
    int __e = EXPECT; \
    if(__a != __e) {    \
        printf("\n%s:%d: assert_eq: \n",  __FILE__, __LINE__);  \
        printf("expect: %d, actual: %d",__e, __a);    \
        while(1);     \
    } \
    }

#else
#define ASSERT(VALUE, ...) \
    if(!(VALUE)) {    \
        printf(__VA_ARGS__);    \
    } 

#define d_printf(...) 
#define d_printfln(...)
#define ASSERT_EQ(EXPECT, ACTUAL, ...) 
#endif




#endif