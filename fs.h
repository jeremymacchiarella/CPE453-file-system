
#define BLOCKSIZE 256

#define DEFAULT_DISK_SIZE 10240 //this is 40 blocks
#define DEFAULT_DISK_NAME “tinyFSDisk”

int tfs_mkfs(char *filename, int nBytes);
int tfs_mount(char *diskname); 
int tfs_unmount(void);

int tfs_openFile(char *name); //RETURNS FILE DESCRIPTOR

int tfs_closeFile(int fd);  
int tfs_writeFile(int fd,char *buffer, int size);

int tfs_deleteFile(int fd); 

int tfs_readByte(int fd, char *buffer);

int tfs_seek(int fd, int offset);
