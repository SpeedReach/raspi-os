#include "initrd.h"
#include "strings.h"
#include "mini_uart.h"
#include "printf.h"



int 
is_last_entry(const cpio_newc_entry_t * const entry){
    return strcmp(entry->data, "TRAILER!!!")  == 0;
}

size_t
entry_size(const cpio_newc_entry_t * const entry){
	return ALIGN(CPIO_HEADER_SIZE + hex2u32_8(entry->header.c_namesize), 4) + ALIGN(hex2u32_8(entry->header.c_filesize), 4);
}

const char*
file_data(const cpio_newc_entry_t * const entry){
    if(hex2u32_8(entry->header.c_filesize) == 0){
        return '\0';
    }
	return ((const  char*) entry) + ALIGN(CPIO_HEADER_SIZE+hex2u32_8(entry->header.c_namesize),4);
}

void print_name(cpio_newc_entry_t* entry) { printf("%s\n", PATH_NAME(entry)); }

void list_files(){
    FOR_FILE(entry, 
        printf("%s\n", PATH_NAME(entry));
    )
}

cpio_newc_entry_t* find_file(char* file_name){
    cpio_newc_entry_t* f = NULL;
    FOR_FILE(entry, 
        if(strcmp(file_name, PATH_NAME(entry)) == 0){
            f = entry;
            break;
        }
    )
    return f;
}