#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libdisk.h"
#include "fs.h"

extern FILE* mounted;

//should i close file in openDisk?

//free block represented as 1 binary, occupied represented as 0 binary
int diskNum = -1;

int diskSizes[MAXDISKS]; //maps disk number to number if bytes allocated for it
char diskNames[MAXDISKS][MAXFILENAME];
FILE* fps[MAXDISKS]; //indexed by disk num, returns file pointer
//need array of file pointers, keep them open

void diskInit(){
    for (int i = 0; i < MAXDISKS; i++){
        diskSizes[i] = -1;
    }
}
int closeDisk(int disk){
    FILE* f = mounted;
    fclose(f);
    return 0;
}

int openDisk(char *filename, int nBytes){
    FILE *f = fopen(filename, "w+");
    
    if (!f){
        printf("failed to open file\n");
        return -1;
    }
    int blocks = nBytes / BLOCKSIZE;
    //to get file size, just write zeros into it nbytes times
    if (blocks == 0){ //means it already exists????
        printf("zero bytes passed in opendisk\n");
        return -1;
    }

    fseek(f, nBytes - 1, SEEK_SET);
    fputc('\0', f);  // this makes the file `nBytes` long with zeros
    
    
    diskNum++;
    fps[diskNum] = f;
    strcpy(diskNames[diskNum], filename);
    diskSizes[diskNum] = blocks;
    
    return diskNum;
    
}

int writeBlock(int disk, int bNum, void *block){
    if (diskSizes[disk] == -1){
        printf("disk %d is not open\n", disk);
        return -1;
    }
    if (diskSizes[disk] <= (bNum)){
        printf("writeblock: bNum %d is not allocated in disk %d\n", bNum, disk);
        return -1;
    }
    int byteNum = bNum * BLOCKSIZE;
    // char filename[MAXFILENAME];
    // strcpy(filename, diskNames[disk]);

    //FILE* f = fopen(filename, "r+");
    //FILE* f = fps[disk]; //change to mounted?
    FILE* f = mounted;
    fseek(f, byteNum, SEEK_SET);
    fwrite(block, BLOCKSIZE, 1, f);
    //fclose(f);
    return 0;
}

int readBlock(int disk, int bNum, void *block){
    
    if (diskSizes[disk] == -1){
        printf("disk %d is not open\n", disk);
        return -1;
    }
    if (diskSizes[disk] <= (bNum)){
        printf("readblock: bNum %d is not allocated in disk %d\n", bNum, disk);
        return -1;
    }

    int byteNum = bNum * BLOCKSIZE;
    
    FILE* f = mounted;
    fseek(f, byteNum, SEEK_SET);
    fread(block, BLOCKSIZE, 1, f); //maybe check this return val?

    //fclose(f); 
    return 0;


}

FILE* getfp(char *diskName){
    for (int i = 0; i < MAXDISKS; i++){
        if (strcmp(diskName, diskNames[i]) == 0){
            return fps[i];
        }
    }
    return NULL; //failed to find disk with such a name
}

int getDiskNum(char *diskName){
    for (int i = 0; i < MAXDISKS; i++){
        if (strcmp(diskName, diskNames[i]) == 0){
            return i;
        }
    }
    return -1; //failed to find a disknum with such a name
}

int getSize(int diskNum){
    return diskSizes[diskNum];
}
