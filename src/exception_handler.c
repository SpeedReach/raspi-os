#include "printf.h"
#include "utils.h"

void default_exception_handler(long long a, long long b, long long c){
    printf("values: %x %x %x\n", a,b,c);
}

