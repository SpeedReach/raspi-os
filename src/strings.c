
#include "strings.h"
#include <stdint.h>
#include <stddef.h>
#include "debug.h"

uint32_t
strlen(const char *const str)
{
    uint32_t len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}


uint32_t 
hex2u32_8(const char *buf){
    uint32_t num = 0;
    for(int i = 0; i < 8; i++){
        num <<= 4;
        num += (buf[i] >= 'A' ? buf[i] - 'A' + 10 : buf[i] - '0');
    }
    return num;
}


int
strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2++)
		if (*s1++ == '\0')
			return (0);
	return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
}

int
strcpy(char* dest, const char* src, int sourceLen){
    if(sourceLen == 0){
        dest [0] = '\0';
        return 0;
    }

    int index = 0;
    while (1)
    {
        char c = src[index];
        dest[index] = c;
        if(c == '\0'){
            break;
        }
        index ++;
        if(index >= sourceLen){
            dest[index] = '\0';
            break;
        }
    }
    return index + 1;
}

int 
atoi(const char* str,int maxLength)
{
    int res = 0;
 
    for (int i = 0; str[i] != '\0' && i <maxLength; ++i)
        res = res * 10 + str[i] - '0';
 
    // return result.
    return res;
}

void memcpy(void *dest, void *src, size_t n){  
// Typecast src and dest addresses to (char *)  
    char *csrc = (char *)src;  
    char *cdest = (char *)dest;  
  
    // Copy contents of src[] to dest[]  
    for (int i=0; i<n; i++)  
        cdest[i] = csrc[i];  
} 


char* strtok(char* str, const char* delim){
    static char* saved = NULL;
    if(str != NULL){
        saved = str;
    }
    if(saved == NULL){
        return NULL;
    }
    if(saved[0] == '\0'){
        saved = NULL;
        return NULL;
    }
    char* start = saved;
    while(*saved != '\0'){
        const char* d = delim;
        while(*d != '\0'){
            if(*saved == *d){
                *saved = '\0';
                saved++;
                return start;
            }
            d++;
        }
        saved++;
    }
    return start;
}

void memset(void* dest, char c, size_t n){
    char* d = (char*) dest;
    for(int i = 0; i < n; i++){
        d[i] = c;
    }
}