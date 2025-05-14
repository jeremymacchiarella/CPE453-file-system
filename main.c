#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libdisk.h"
#include "fs.h"

int main(int argc, char **argv){
    diskInit();
    tfs_mkfs("disk0.txt", BLOCKSIZE * 5);
    
    return 0;
}