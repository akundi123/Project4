#include "sfs.h"
#include <stdio.h>
#include <string.h>
#include <errno.h> // to indicate errors
#include <assert.h> //for validation

simple fileS;
int8_t b[BLOCK_SIZE];

int make_sfs(char *disk_name)//creates the new file system
{
    int fd = open_disk(disk_name);
    if (fd < 0)
        return -1; //error
    
    strcpy((char*)fileS.def.nmu_m, MNUM);
    fileS.def.vnum = 1;
    memset(fileS.def.blocks, 0, sizeof(fileS.def.blocks));
    
    fileS.def.blocks[0] = 1;
    memset(fileS.def.f, 0, sizeof(fileS.def.f));
    memset(b, 0, sizeof(b));
    memcpy(b, &fileS.def,  sizeof(fileS.def));
    
    write_block(fd, 0, (char*)b);
    return close_disk(fd);
}

int mount_sfs(char *disk_name)//mounts the disk
{
    int fd = open_disk(disk_name);
    if (fd < 0)
        return -1;
    fileS.open = 1;
    fileS.fdd = fd;
    memset(&fileS.fd1, 0, sizeof(fileS.fd1));
    if (read_block(fileS.fdd, 0, (char*)b) < 0)
        return -1;
    memcpy(&fileS.def, b, sizeof(fileS.def));
    return 0;
}

int umount_sfs(char *disk_name)
{
    if (!fileS.open)
        return -1;
    memset(b, 0, sizeof(b));
    memcpy(b, &fileS.def,  sizeof(fileS.def));
    if (write_block(fileS.fdd, 0, (char*)b) < 0)
        return -1;
    fileS.open = 0;
    return 0;
}

int allocation_function()
{
    int i, j;
    for (i = 0; i < sizeof(fileS.def.blocks); i++)
    {
        if (fileS.def.blocks[i] != -1)
        {
            int32_t bmap = fileS.def.blocks[i];
            for (j = 0; j < 32; j++)
            {
                if ((bmap & (1 << j)) == 0)
                {
                    int32_t bnum = (i << 5) + j;
                    fileS.def.blocks[i] |= (1 << j);
                    memset(b, 0, sizeof(b));
                    write_block(fileS.fdd, bnum, (char*)b);
                    return bnum;
                }
            }
        }
    }
    return -1;
}

int allocation_removal(int bnum)
{
    if (bnum <= 0 || bnum >= MAX_BLOCKS)
        return -1;
    int32_t bmap = bnum / 32;
    int32_t read = bnum % 32;
    assert(fileS.def.blocks[bmap] & (1 << read));
    fileS.def.blocks[bmap] ^= (1 << read);
    return 0;
}

int location(char *f, int create)
{
    int i;
    for (i = 0; i < MAX_FILES; i++)
    {
        if (!fileS.def.f[i].check)
            break;
        if (strcmp((char*)fileS.def.f[i].n, f) == 0)
            return i;
    }
    
    if (create && i < MAX_FILES)
    {
        fileS.def.f[i].check = 1;
        strcpy((char*)fileS.def.f[i].n, f);
        fileS.def.f[i].fnum = allocation_function();
        fileS.def.f[i].sizef = 0;
        return i;
    }
    return -1;
}

int sfs_open(char *file_name)
{
    if (!fileS.open || file_name == NULL)
        return -1;
    int entry = location(file_name, 0);
    if (entry < 0)
        return -1;
    
    int num;
    for (num = 0; num < MAX_OPEN_FILES; num++)
    {
        if (!fileS.fd1[num].open)
            break;
    }
    if (num == MAX_OPEN_FILES)
        return -1;
    
    fileS.fd1[num].entry = entry;
    fileS.fd1[num].sk = 0;
    fileS.fd1[num].sizef = fileS.def.f[entry].sizef;
    fileS.fd1[num].fnum = fileS.def.f[entry].fnum;
    int32_t real = fileS.fd1[num].fnum;
    if (read_block(fileS.fdd, real, (char*)&fileS.fd1[num].def) < 0)
        return -1;
    fileS.fd1[num].open = 1;
    return num;
}

int32_t n(int entry, int32_t bnum)
{
    int i;
    int32_t real = fileS.def.f[entry].fnum;
    for (i = 0; i < bnum; i++)
    {
        read_block(fileS.fdd, real, (char*)b);
        real = *((int8_t*)b);
        if (real <= 0)
            return 0;
    }
    return real;
}

int sfs_close(int fd)
{
    if (!fileS.open || fd < 0 || fd >= MAX_OPEN_FILES || !fileS.fd1[fd].open)
        return -1;
    
    int32_t entry = fileS.fd1[fd].entry;
    int32_t b = fileS.fd1[fd].sk / BYTE_SIZE;/////////////////////////
    int32_t real = n(fileS.fd1[fd].entry, b);
    
    if (write_block(fileS.fdd, real, (char*)&fileS.fd1[fd].def) < 0)
        return -1;
    fileS.def.f[entry].sizef = fileS.fd1[fd].sizef;
    fileS.fd1[fd].open = 0;
    return 0;
}

int sfs_create(char *file_name)
{
    if (!fileS.open || file_name == NULL)
        return -1;
    int entry = location(file_name, 0);
    if (entry >= 0)
        return -1;
    entry = location(file_name, 1);
    if (entry < 0)
        return -1;
    return 0;
}

int sfs_delete(char *file_name)
{
    if (!fileS.open || file_name == NULL)
        return -1;
    int entry = location(file_name, 0);
    if (entry < 0)
        return -1;
    int32_t bnum = fileS.def.f[entry].fnum;
    while(allocation_removal(bnum) == 0)
    {
        read_block(fileS.fdd, bnum, (char*)b);
        bnum = *((int32_t*)b);
    }
    fileS.def.f[entry].check = 0;
    return 0;
}

int sfs_read(int fd, void *buf, size_t count)
{
    if (!fileS.open || fd < 0 || fd >= MAX_OPEN_FILES || !fileS.fd1[fd].open)
        return -1;
    int32_t read = fileS.fd1[fd].sk;
    int32_t b = read / BYTE_SIZE;
    int32_t final = 0;
    if (read + count > fileS.fd1[fd].sizef)
        count = fileS.fd1[fd].sizef - read;
    
    read %= BYTE_SIZE;
    while(count > 0)
    {
        int32_t num_byte = BYTE_SIZE - read;////////////////
        if (num_byte == 0)
        {
            int32_t real1 = fileS.fd1[fd].def.more;
            assert(real1 > 0);
            if (read_block(fileS.fdd, real1, (char*)&fileS.fd1[fd].def) < 0)
                break;
            read = 0;
            b++;
            num_byte = BYTE_SIZE;
        }
        if (num_byte > count)
            num_byte = count;
        memcpy(buf, fileS.fd1[fd].def.read + read, num_byte);
        buf += num_byte;
        count -= num_byte;
        read += num_byte;
        final += num_byte;
    }
    read += b * BYTE_SIZE;///////////////
    fileS.fd1[fd].sk = read;
    return final;
}

int sfs_write(int fd, void *buf, size_t count)
{
    if (!fileS.open || fd < 0 || fd >= MAX_OPEN_FILES || !fileS.fd1[fd].open)
        return -1;
    int32_t read = fileS.fd1[fd].sk;
    int32_t b = read / BYTE_SIZE;/////////
    int32_t final = 0;
    read %= BYTE_SIZE;//////////
    while(count > 0)
    {
        int32_t num_byte = BYTE_SIZE - read;////////
        if (num_byte == 0)
        {
            int32_t real = n(fileS.fd1[fd].entry, b);
            int32_t real1 = fileS.fd1[fd].def.more;
            if (real1 <= 0)
            {
                if (b == MAX_DATA_BLOCKS)
                    break;
                real1 = allocation_function();
                fileS.fd1[fd].def.more = real1;
                if (real1 <= 0)
                    break;
            }
            if (write_block(fileS.fdd, real, (char*)&fileS.fd1[fd].def) < 0)
                break;
            if (read_block(fileS.fdd, real1, (char*)&fileS.fd1[fd].def) < 0)
                break;
            read = 0;
            b++;
            num_byte = BYTE_SIZE;
        }
        if (num_byte > count)
            num_byte = count;
        memcpy(fileS.fd1[fd].def.read + read, buf, num_byte);
        buf += num_byte;
        count -= num_byte;
        read += num_byte;
        final += num_byte;
    }
    read += b * BYTE_SIZE;//////
    fileS.fd1[fd].sk = read;
    if (read > fileS.fd1[fd].sizef)
        fileS.fd1[fd].sizef = read;
    return final;
}

int sfs_seek(int fd, int offset)
{
    if (!fileS.open || fd < 0 || fd >= MAX_OPEN_FILES || !fileS.fd1[fd].open)
        return -1;
    if (offset >= fileS.fd1[fd].sizef)
        return -1;
    int32_t rn = fileS.fd1[fd].sk;
    int32_t b1 = offset / BYTE_SIZE;//////////
    int32_t brn = rn / BYTE_SIZE;////////
    if (brn != b1)
    {
        int32_t real1 = n(fileS.fd1[fd].entry, b1);
        int32_t real = n(fileS.fd1[fd].entry, brn);
        if (write_block(fileS.fdd, real, (char*)&fileS.fd1[fd].def) < 0)
            return -1;
        if (read_block(fileS.fdd, real1, (char*)&fileS.fd1[fd].def) < 0)
            return -1;
    }
    fileS.fd1[fd].sk = offset;
    return 0;
}
