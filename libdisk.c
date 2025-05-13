#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libdisk.h"

//should i close file in openDisk?

int diskNum = -1;

int diskSizes[MAXDISKS]; //maps disk number to number if bytes allocated for it
char diskNames[MAXDISKS][MAXFILENAME];

void diskInit(){
    for (int i = 0; i < MAXDISKS; i++){
        diskSizes[i] = -1;
    }
}
int closeDisk(int disk){
    char name[MAXFILENAME];
    strcpy(name, diskNames[disk]);
    diskSizes[disk] = -1;
    int res;
    if ((res = remove(name)) < 0){
        printf("failed to remove file %s\n", name);
        exit(1);
    }
    return 0;
}

int openDisk(char *filename, int nBytes){
    FILE *f = fopen(filename, "w");
    if (!f){
        printf("failed to open file\n");
        return -1;
    }
    int blocks = nBytes / BLOCKSIZE;
    
    if (blocks == 0){
        printf("zero bytes passed in opendisk\n");
        return -1;
    }

    
    //TODO: ALLOCATE NbYTES FOR THE FILE SOMEHOW

    diskNum++;
    strcpy(diskNames[diskNum], filename);
    diskSizes[diskNum] = blocks;
    fclose(f); //maybe dont close file here? !!!!!!!!!!!!!!!!!!!!!!!!!
    return diskNum;
    
}

int writeBlock(int disk, int bNum, void *block){
    if (diskSizes[disk] == -1){
        printf("disk %d is not open\n", disk);
        return -1;
    }
    if (diskSizes[disk] <= (bNum)){
        printf("bNum %d is not allocated in disk %d\n", bNum, disk);
        return -1;
    }
    int byteNum = bNum * BLOCKSIZE;
    char filename[MAXFILENAME];
    strcpy(filename, diskNames[disk]);

    FILE* f = fopen(filename, "r+");
    fseek(f, byteNum, SEEK_SET);
    fwrite(block, BLOCKSIZE, 1, f);
    fclose(f);
    return 0;
}

int readBlock(int disk, int bNum, void *block){
    if (diskSizes[disk] == -1){
        printf("disk %d is not open\n", disk);
        return -1;
    }
    if (diskSizes[disk] <= (bNum)){
        printf("bNum %d is not allocated in disk %d\n", bNum, disk);
        return -1;
    }

    int byteNum = bNum * BLOCKSIZE;
    char filename[MAXFILENAME];
    strcpy(filename, diskNames[disk]);

    FILE* f = fopen(filename, "r+");
    fseek(f, byteNum, SEEK_SET);
    fread(block, BLOCKSIZE, 1, f); //maybe check this return val?
    fclose(f); 
    return 0;


}

