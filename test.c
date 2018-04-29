// To compile: gcc test.c -std=c99
// To Run: ./a.out

#include <stdio.h>
#include <stdlib.h>
#include "sfs.c"

char DISK_NAME[5] = "disk"; //File name for disk
char FILE1_NAME[10] = "FILE1.txt";
void test_virtual_disk()
{
  create_disk(DISK_NAME, DISK_NBYTES);
  
  int fd = open_disk(DISK_NAME); 
  printf("File Descriptor: %d\n", fd); //Opened file descriptor
  
  char buf_write[BLOCK_SIZE] = "Hello";
  char buf_read[BLOCK_SIZE];
  
  write_block(fd, 2, buf_write);
  read_block(fd, 2, buf_read);
  printf("Block read: %s\n", buf_read);
  
  close_disk(fd);
}

void test_sfs()
{
  int res = make_sfs(DISK_NAME);
  printf("make_sfs(): %d\n", res); //Showing output of make_sfs
  
  res = mount_sfs(DISK_NAME);
  printf("mount_sfs(): %d\n", res);
 
  sfs_create(FILE1_NAME);
  printf("sfs_create(): %d\n", res);
 
  res = umount_sfs(DISK_NAME);
  printf("umount_sfs(): %d\n", res); 
  
}

int main(void)
{

  test_sfs();
  return 0;
}