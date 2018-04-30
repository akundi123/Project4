// To compile: gcc test.c -std=c99
// To Run: ./a.out

#include <stdio.h>
#include <stdlib.h>
#include "sfs.c"

char DISK_NAME[5] = "disk"; //File name for disk
char FILE1_NAME[10] = "FILE1.txt";
char FILE2_NAME[10] = "FILE2.txt";

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
  //int res = make_sfs(DISK_NAME);
  //printf("make_sfs(): %d\n", res); //Showing output of make_sfs
  int res;
  res = mount_sfs(DISK_NAME);
  printf("mount_sfs(): %d\n", res);
  
  print_root_directory(); 
 
  res = sfs_create(FILE1_NAME);
  printf("sfs_create(): %d\n", res);
 
  int file_descriptor = sfs_open(FILE1_NAME);
 
  char buf_write[BLOCK_SIZE] = "Hello";
  char buf_read[BLOCK_SIZE];

  res = sfs_write(file_descriptor, buf_write, 5);
  printf("sfs_write(): %d\n", res);
 
  res = sfs_read(file_descriptor, buf_read ,5);
  printf("Buf_read: %s\n", buf_read);
  
  int offset = 2;
  res = sfs_seek(file_descriptor, offset);
  printf("sfs_seek: %d\n", res);
  
  res = sfs_close(file_descriptor);
  printf("sfs_close: %d\n", res);
  
  res = sfs_delete(FILE1_NAME);
  printf("sfs_delete: %d\n", res);
 
  
  // Testing to see if File2.txt is still there when remounted
  res = sfs_create(FILE2_NAME);
  int file_descriptor2 = sfs_open(FILE2_NAME);
  res = sfs_write(file_descriptor2, buf_write, 5);
 
  res = umount_sfs(DISK_NAME);
  printf("umount_sfs(): %d\n", res); 

  res = mount_sfs(DISK_NAME);
  printf("mount_sfs(): %d\n", res);
  print_fat();
  
}

int main(void)
{

  test_sfs();
  return 0;
}