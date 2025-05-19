#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libdisk.h"
#include "fs.h"

int main(int argc, char **argv){
    diskInit();
    
    int diskNum = tfs_mkfs("disk0.txt", BLOCKSIZE * 40);
    printf("disknum: %d\n", diskNum);
    if (diskNum < 0){
        printf("error creating disk\n");
    }

   

    int fd0 = tfs_openFile("file0");
    int fd1 = tfs_openFile("file1");
    printf("fd0: %d\n", fd0);
    printf("fd1: %d\n", fd1);
    return 0;
}