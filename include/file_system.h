

#ifndef	FILE_SYSTEM_H
#define	FILE_SYSTEM_H

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
} cpio_newc_header;

typedef struct 
{
	char* name;
    char* data;
	//chars
	unsigned int nameLength;
	//chars
	unsigned int fileLength;
}cpio_file;

#define FILE_SYSTEM_BASE 0x8000000


static cpio_file* files[100];
static int fileCount = 0;

void load_file_system(void);

#endif  /*FILE_SYSTEM_H */