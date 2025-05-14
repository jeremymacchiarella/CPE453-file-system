#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libdisk.h"
#include "fs.h"

#define MAXFILES 64

int offset[MAXFILES]; //indexed by fd, value is current offset to read and write

//need dynamic array to store shit???
FILE* mounted = NULL;
int tfs_mkfs(char *filename, int nBytes){

    /////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////

    int blocks = nBytes/BLOCKSIZE;
    int superBlockNum = openDisk(filename, nBytes);
    if (superBlockNum < 0){
        printf("tfs_mkfs fail\n");
        return -1;
    }
    char superblock[BLOCKSIZE]; //need to encode where the free blocks are in the superblock
    //probably do indexed
    uint8_t type = 1;
    uint8_t magicNum = 0x44;
    uint32_t free = 0xFFFFFFFF; //all blocks are free at the start 
    memcpy(superblock, &type, 1);
    memcpy(&superblock[1], &magicNum, 1);
    memcpy(&superblock[4], &free, 4);
    memcpy(&superblock[8], &free, 1);

    

    //start at superblock[4] for data
    
    
    
    
    mounted = getfp(filename);
    fseek(mounted, 0, SEEK_SET);
    writeBlock(superBlockNum, 0, superblock);
    

    return 0;

}
int tfs_mount(char *diskname){
    FILE *f = getfp(diskname);
    mounted = f;
    char block[BLOCKSIZE];
    fread(block, BLOCKSIZE, 1, f);
    uint8_t type, magicNum;
    memcpy(&type, block, 1);
    memcpy(&magicNum, &block[1], 1);
    if (magicNum != 0x44){
        printf("magic number is wrong in mount\n");
        return -1;
    }
    //NEED SOME LOGIC HERE PROBABLY TO MOUNT
    //MAYBE HAVE EXTERN VAR THAT SAYS CURR MOUNTED DISK
    //OR A GETTER FUNCTION FOR THE CURR MOUNTED DISK

    //extern pointer to a file that all functions in libdisk use??
    return 0;
}

int tfs_unmount(void){
    mounted = NULL;
    if (!mounted){
        printf("unmounting null\n");
        return -1;
    }
    return 0;
}

int tfs_openFile(char *name){ //RETURNS FILE DESCRIPTOR
    //create inode if not existing already
} 

