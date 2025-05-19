#define BLOCKSIZE 256
#define MAXDISKS 32
#define MAXFILENAME 9

int openDisk(char *filename, int nBytes);

int closeDisk(int disk);
int readBlock(int disk, int bNum, void *block);
int writeBlock(int disk, int bNum, void *block);
void diskInit();
FILE* getfp(char *diskName);
int getDiskNum(char *diskName);
int getSize(int diskNum);