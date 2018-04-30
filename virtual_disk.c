#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 4096
#define MAX_DATA_BLOCKS 3500
#define MAX_FILES 64
#define MAX_OPEN_FILES 64
#define FNAME_LENGTH 16

// Number of bytes for disk
const size_t DISK_NBYTES = BLOCK_SIZE * MAX_BLOCKS;

// Looks like RLIMT_FSIZE IS 1600 bytes
int create_disk(char* filename, size_t nbytes)
{ 
  
  FILE *fp;
  
  fp = fopen( "disk" , "w" );
  if (fp == NULL) return -1; // File not opened correctly
  
  char zero[1] ={'0'}; // one byte
  
  //Initalize with empty blocks for each byte
  for (int block_num = 0; block_num < MAX_BLOCKS; block_num++)
  {
    // Making one block with 4, 1024 b/c 1600 write limit
    for (int quarter_block = 0; quarter_block < 4; quarter_block++) 
    {
      fwrite(zero , 1 , 1024 , fp );
    }  
  }

  fclose(fp);
  return 0; // Disk Created Succesfully
  
}

int open_disk(char* filename)
{
  int file_descriptor = open(filename, O_RDWR);
  return file_descriptor;
}

int read_block(int disk, int block_num, char *buf)
{
  // Start at this byte number
  size_t byte_start = block_num * BLOCK_SIZE;
  off_t offest = lseek(disk, byte_start, SEEK_SET);
  
  size_t bytes_read = read(disk, buf, BLOCK_SIZE);
  
  //printf("Bytes read: %d\n", bytes_read); 
  return bytes_read;
}

int write_block(int disk, int block_num, char *buf)
{
  // Start at this byte number
  size_t byte_start = block_num * BLOCK_SIZE;
  
  int bytes_written = 0;
  // Making one block with 4 x 1024 b/c 1600 write limit
  for (int quarter_block = 0; quarter_block < 4; quarter_block++) 
  {
    off_t offest = lseek(disk, byte_start + (1024 * quarter_block), SEEK_SET);
    bytes_written+= write(disk, buf, 1024);
  } 

  return bytes_written; 
}

int close_disk(int disk)
{
 close(disk);
 return 0;
}