#include "printf.h"
#include "file_system.h"
#include "strings.h"
#include "stddef.h"
#include "simple_allocator.h"
#include "utils.h"
#include "atoi.h"

cpio_file* parse_file(const void *const start);

int cpioFileLen(cpio_file* file){
    int len = CPIO_HEADER_SIZE;
    len += (file->nameLength + file->nameLength%2)*sizeof(char);
    if(file->fileLength > 0){
        len += file->fileLength + 1;
        len += (file->fileLength +1 )%2;
    }
    return len;
}

void load_file_system(void){
    char* cur = (char*) FILE_SYSTEM_BASE;
    while(1){
        while(*cur == '\0'){
            cur ++;
        }
        cpio_file* file = parse_file(cur);
        cur += cpioFileLen(file);
        if(file == NULL){
            break;
        }
        files[fileCount] = file;
        fileCount ++;
        printf("%d\n", fileCount);
    }
    return;
}



/**
 * @brief parses a New ASCII Format file
 * 
 * @param start start of the file stream
 * @return cpio_file*  returns NULL if fails parsing
 */
cpio_file* parse_file(const void *const start){
    
    cpio_newc_header header = *(cpio_newc_header*)start;
    

    if(atoi(header.c_magic,6) != 70701){
        printf("Failed to load file, magic value not 70701 but %d\n",atoi(header.c_magic,6));
        return NULL;
    }
    //chars
    const int nameLength = hex2u32_8(header.c_namesize);
    const int fileLength = hex2u32_8(header.c_filesize);

    cpio_file* file = simple_malloc(sizeof(cpio_file));
    file->name = simple_malloc(nameLength * sizeof(char));
    file->data = simple_malloc(fileLength * sizeof(char));
    file->nameLength = nameLength;
    file->fileLength = fileLength;

    char* stream = (char*) (start + CPIO_HEADER_SIZE);
    
    strcpy(file->name, stream, nameLength);
    if(strcmp(file->name,"TRAILER!!!") == 0){
        return NULL;
    }
    stream += nameLength + (nameLength % 2);
    strcpy(file->data, stream, fileLength);
    return file;
}