// To compile: gcc test.c -std=c99
// To Run: ./a.out

#include <stdio.h>
#include "virtual_disk.c"


char * DISK_NAME = "disk"; //File name for disk
size_t disk_nbytes = BLOCK_SIZE * MAX_BLOCKS; // Number of bytes for disk

int main(void)
{
  create_disk(DISK_NAME, disk_nbytes);
  
  int fd = open_disk(DISK_NAME); 
  printf("File Descriptor: %d\n", fd); //Opened file descriptor
  
  char buf_write[BLOCK_SIZE] = "Hello";
  char buf_read[BLOCK_SIZE];
  
  write_block(fd, 2, buf_write);
  read_block(fd, 2, buf_read);
  printf("%s", buf_read);
  
  close_disk(fd);
  
  return 0;
}