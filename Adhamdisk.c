#include "disk.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int create_disk(char* filename, size_t nbytes)
{
    int fd = open(filename, O_WRONLY | O_CREAT, 0600);
    //O_WRONLY: open for writing only
    //O_CREATE: If the file exists, this flag has no effect except as noted under O_EXCL below. Otherwise, the file is created;
    
    if (fd < 0) //unable to open throws error
        return -1;//error
    
    if (posix_fallocate(fd, 0, nbytes))//posix_fallocate ensures that disk space is allocated.
        return -1;//error

    close(fd);//closes fd
    return 0;//success
}

int open_disk(char* filename)
{
    return open(filename, O_RDWR);
}
int read_block(int disk, int block_num, char *buf)
{
    if (lseek(disk, block_num * BLOCK_SIZE, SEEK_SET) < 0)//check seeking errors.
    {
        printf("Unable to lseek \n");
        return -1;//error
    }
    if (read(disk, buf, BLOCK_SIZE) != BLOCK_SIZE)//checks reading errors.
    {
        printf("Unable to read\n");
        return -1;//error
    }
    return 0;//success
}
int write_block(int disk, int block_num, char *buf)
{
    if (lseek(disk, block_num * BLOCK_SIZE, SEEK_SET) < 0)//checks seeking errors.
    {
        printf("Unable to lseek \n");
        return -1; //error
    }
    if (write(disk, buf, BLOCK_SIZE) != BLOCK_SIZE)//checks writing errors.
    {
        printf("Unable to write\n");
        return -1; //error
    }
    return 0;//success
}
int close_disk(int disk)
{
    return close(disk);
}
