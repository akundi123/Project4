#include "virtual_disk.c"
#include <string.h>

char disk_mount[BLOCK_SIZE * MAX_BLOCKS];
int disk_fd;

int make_sfs(char *disk_name)
{
  create_disk(disk_name, DISK_NBYTES);
  int disk_fd = open_disk(disk_name);
  close_disk(disk_fd);
  if (disk_fd>0) return 0;
  else return -1; 
}

int mount_sfs(char *disk_name)
{
  int fd = open_disk(disk_name);
  for (int i = 0; i < 4096; i++)
  {
    read_block(fd, i, disk_mount);
  }
  printf("Disk_mount: %s\n", disk_mount);
  
  return 0;
  //open all blocks into memory
  
}

int umount_sfs(char *disk_name) 
{
  //Write from memory to disk
  
  //Close the disk
  return close_disk(disk_fd);
}


// Access rotuines //
int sfs_create(char *file_name)
{
  if (strlen(file_name) > FNAME_LENGTH) return -1;
 
  //Check if file is present
  
  //Set a space in the earliest avaible block for the file
  return 0;
}