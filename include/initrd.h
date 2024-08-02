
#ifndef _INITRD_H
#define _INITRD_H

#include <stddef.h>
#include <stdint.h>
#include "endian.h"
#include "printf.h"
#include "strings.h"
#include "utils.h"

#define CPIO_HEADER_SIZE 6+13*8

typedef struct  {
    char    c_magic[6];
	char    c_ino[8];
	char    c_mode[8];
	char    c_uid[8];
	char    c_gid[8];
	char    c_nlink[8];
	char    c_mtime[8];
	char    c_filesize[8];
    char    c_devmajor[8];
	char    c_devminor[8];
	char    c_rdevmajor[8];
	char    c_rdevminor[8];
	char    c_namesize[8];
	char    c_check[8];
} cpio_newc_header_t;


typedef struct {
    cpio_newc_header_t 	header;
    char 				data[];
} cpio_newc_entry_t;




void initramfs();

#define IS_MAGIC_VALUE(VALUE) \
	value[0] == '0' &&	\
    value[1] == '7' &&	\
    value[2] == '0' &&	\
    value[3] == '7' &&	\
    value[4] == '0' &&	\
    value[5] == '1'

#define PATH_NAME(VALUE) \
	((const cpio_newc_entry_t * const) VALUE)->data

const char*
file_data(const cpio_newc_entry_t * const entry);

size_t
entry_size(const cpio_newc_entry_t * const entry);

int
is_last_entry(const cpio_newc_entry_t * const entry);

void 
list_files();

extern char* __initramfs_start;

#define FOR_FILE( ENTRY ,ACTION ) \
    char* cursor = __initramfs_start; \
    printf("aa: %x\n", cursor);\
    while (1)	\
    {	\
        cpio_newc_entry_t * ENTRY = (cpio_newc_entry_t * ) cursor;	\
        if(is_last_entry(ENTRY)){	\
            break;	\
        }	\
        size_t size = entry_size(ENTRY);	\
        if(size == 0){	\
            break;	\
        }	\
		ACTION	\
        cursor += size;	\
    }	\


cpio_newc_entry_t* find_file(char* file_name);

#endif