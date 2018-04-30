#include <stdio.h>
#include <stdlib.h>
#ifndef disk_h
#define disk_h
#endif

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 4096

int create_disk(char* filename, size_t nbytes);
int open_disk(char* filename);
int read_block(int disk, int block_num, char *buf);
int write_block(int disk, int block_num, char *buf);
int close_disk(int disk);
