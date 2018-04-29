#include "virtual_disk.c"
#include <string.h>
#include <stdio.h>

char disk_mount[BLOCK_SIZE * MAX_BLOCKS];
int disk_fd;

// Root directory

// FAT table
// Assuming Blocks 4096 - 3500 are used to store the file system
// Only the last 3500 can be data blocks 
// 0xFFFFFFFF is end of file
// 0x00000000 is not allocated
// File Descriptor can be 0 to MAX_FILES
// Fiel Descriptor of -1 is not referneced
typedef struct file{
  char name[FNAME_LENGTH];
  int start_cluster;
  int length;
  int offset;
  int file_descriptor;
} file_t;


int * file_allocation_table[MAX_DATA_BLOCKS] = {0x00000000};  // Set them all to free
file_t inital = {.name="", .start_cluster=-1, .length=-1, .file_descriptor=-1}; 
file_t root_directory[MAX_FILES];

// Keep track of which blocks are used in the file
typedef struct data_node{
  int block_in_use;
} data_node;

// Linked list to keep track of files
typedef struct file_node{
    // Pointer to blocks that have data
    
    // The offest of the data node is current_file_offest % BLOCK_SIZE
    int current_file_offest; 
    struct file_node* next;
} file_node;

// Structure to impliment common operations

file_node files; 

// Helpers //
int earliest_free_block()
{
  for (int i = 0; i < MAX_DATA_BLOCKS; i++)
  {
    if (file_allocation_table[i] == 0) return i;
  }
}

int earliest_file_available()
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(strlen(root_directory[i].name) == 0)
    {
      return i;
    }
  }
}

int get_start_cluser(int file_descriptor)
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(file_descriptor == root_directory[i].file_descriptor)
    {
      return root_directory[i].start_cluster;
    }
  }
}

// Returns -1 if there are no free files to be opened
int get_earliest_file_descriptor()
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(root_directory[i].file_descriptor <= 0)
    {
      return i;
    }
  }
  return -1;
}

int set_file_descriptor(char*file_name, int file_descriptor)
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(strcmp(file_name, root_directory[i].name) == 0)
    {
    root_directory[i].file_descriptor = file_descriptor;
    return 0;
    }
  }
  return -1;
}

int close_file_descriptor(int file_descriptor)
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(root_directory[i].file_descriptor == file_descriptor)
    {
      root_directory[i].file_descriptor = -1;
      return 0;
    }
  }
  return -1;
}

file_t get_file(int file_descriptor)
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(root_directory[i].file_descriptor == file_descriptor)
    {
      return root_directory[i];
    }
  }
  return root_directory[0];
}

void print_root_directory()
{
for (int i = 0; i < MAX_FILES; i++)
  {
    file_t file = root_directory[i];
    printf("Name: %s, File Descriptor: %d, Offest: %d \n",file.name, file.file_descriptor, file.offset);
  }
}

void print_fat()
{
for (int i = 0; i < 5; i++)
  {
    printf("I: %d, FAT[i]: %x\n", i, file_allocation_table[i]);
  }
}


// Managers // 
int make_sfs(char *disk_name)
{
  create_disk(disk_name, DISK_NBYTES);
  int disk_fd = open_disk(disk_name);
  close_disk(disk_fd);
  
  // Inital root directory structure  
  memset(root_directory, 0, MAX_FILES * sizeof(struct file));
  for (int i = 0; i < MAX_FILES; i++)
  {
    root_directory[i].file_descriptor = -1;
  }
  
  if (disk_fd>0) return 0;
  else return -1; 
}

int mount_sfs(char *disk_name)
{
  disk_fd = open_disk(disk_name);
  for (int i = 0; i < 4096; i++)
  {
    read_block(disk_fd, i, disk_mount);
  }
  
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
  
  //Make a new entry in the  root directory
  //Search for earliest open spot
  int earliest_free_index = earliest_file_available();
  strcpy(root_directory[earliest_free_index].name, file_name);
  
  return 0;
}

int sfs_open(char *file_name)
{
  // Give next avaible file desicriptor
  int file_descriptor = get_earliest_file_descriptor();
  // Return -1 if max files are open
  if (file_descriptor == -1) return -1;
 
  //Set the file name to the earliest descriptor
  set_file_descriptor(file_name, file_descriptor);
}

int sfs_close(int fd)
{
  //Free up file_descriptor (set file_descriptor to -1)
  
  return close_file_descriptor(fd);
}

int sfs_write(int fd, void *buf, size_t count)
{
 //Get Amount of blocks needed to write
 
 //Allocate space in file table
 //Get earliest free  block
 int cluster_start = earliest_free_block();
 //Set the cluster_start to the first table
 file_allocation_table[cluster_start] = cluster_start+1;
 //Setting the end of file marker
 file_allocation_table[cluster_start+1] = "0xFFFFFFFF";
 
 //Write to each block
 //printf("fd%d",disk_fd);
 //fflush(stdout);
 
 write_block(disk_fd, cluster_start, buf);
 return 0;
}

int sfs_read(int fd, void *buf, size_t count)
{

  // Look up start cluster in file table
  int start_cluster = get_start_cluser(fd);
  //printf("Start cluster: %d\n", start_cluster);
  read_block(disk_fd, start_cluster, buf);
 
}

int sfs_delete(char *file_name)
{
  //Reset the file to the blank state
  {
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(strcmp(file_name, root_directory[i].name) == 0)
    {
    strcpy(root_directory[i].name, "");
    root_directory[i].start_cluster = 0;
    root_directory[i].length = 0;
    root_directory[i].file_descriptor = -1;
    return 0;
    }
  }
  return -1;
  }
}
int sfs_seek(int fd, int offset)
{
  for (int i = 0; i < MAX_FILES; i++)
  {
    if(root_directory[i].file_descriptor == fd)
    {
      root_directory[i].offset = 2;
      return 0;
    }
  }
  return -1;
}
