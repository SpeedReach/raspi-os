#ifndef _STRINGS_H
#define _STRINGS_H


#include <stdint.h>
#include <stddef.h>


uint32_t strlen(const char *const str);

int strcmp(const char* s1, const char* s2);
char* strtok(char* str, const char* delim);

//bytes copied into dest
int strcpy(char* dest, const char* src, int sourceLen);

uint32_t hex2u32_8(const char *buf);

int atoi(const char* str,int maxLength);

void memcpy(void *dest, void *src, size_t n);

void memset(void *dest, char val, size_t n);




#endif