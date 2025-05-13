#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libdisk.h"
#include "fs.h"

//need dynamic array to store shit???
int tfs_mkfs(char *filename, int nBytes){
    int diskNum = openDisk(filename, nBytes);
    if (diskNum < 0){
        printf("tfs_mkfs fail\n");
        return -1;
    }
    char superblock[BLOCKSIZE];
    uint8_t type = 1;
    uint8_t magicNum = 0x44;
    memcpy(superblock, &type, 1);
    memcpy(&superblock[1], &magicNum, 1);
    memset(&superblock[2], 0, 254);

}

