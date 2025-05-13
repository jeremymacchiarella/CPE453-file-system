#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libdisk.h"

int main(int argc, char **argv){
    diskInit();
    int d0 = openDisk("disk0.txt", BLOCKSIZE * 5);
    printf("opened disk %d\n", d0);
    char block[BLOCKSIZE] = "Hello world!";
    writeBlock(d0, 0, (void*) block);
    char block2[BLOCKSIZE] = "Writing to block 1 aka the second block";
    writeBlock(d0, 1, (void*) block2);
    writeBlock(d0, 5, block);
    closeDisk(d0);
    return 0;
}