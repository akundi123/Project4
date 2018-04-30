#include <stdio.h>
#include "disk.h"
#include <stdlib.h>
#include <stdint.h>
#ifndef sfs_h
#define sfs_h
#endif

#define MAX_DATA_BLOCKS 3500//
#define MAX_FILES 64//
#define MAX_OPEN_FILES 64//
#define FNAME_LENGTH 16//
#define MNUM "NULL"
#define BYTE_SIZE 4092 // (4096 - sizeof(int32_t)) //--

typedef struct _SFS
{
    int32_t open;
    int32_t fdd;
    
    struct
    {
        int8_t nmu_m[32]; //magic number
        int32_t vnum; //version number to compare
        int32_t blocks[MAX_BLOCKS >> 5]; //--
        
        struct
        {
            int32_t check;
            int8_t n[FNAME_LENGTH];
            int32_t fnum;
            int32_t sizef;
        } f[MAX_FILES];
    } def;
    
    struct
    {
        int32_t open;
        int32_t entry;
        int32_t fnum;
        int32_t sizef;
        int32_t sk;

        struct
        {
            int32_t more;
            int8_t read[BYTE_SIZE];//---
        } def;
    } fd1[MAX_OPEN_FILES]; //---
} simple;

int make_sfs(char *disk_name);
int mount_sfs(char *disk_name);
int umount_sfs(char *disk_name);
int sfs_open(char *file_name);
int sfs_close(int fd);
int sfs_create(char *file_name);
int sfs_delete(char *file_name);
int sfs_read(int fd, void *buf, size_t count);
int sfs_write(int fd, void *buf, size_t count);
int sfs_seek(int fd, int offset);
