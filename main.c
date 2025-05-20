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

    char buffer[20] = "Hello World!";
    char buffer2[10] = "ddddmmmm";

    tfs_writeFile(fd0, buffer, strlen(buffer));
  
    char buff[1];
    int res;
    printf("read data: ");
    while((res = tfs_readByte(fd0, buff)) > 0){
        printf("%c", buff[0]);
    }
    printf("\n");
   

    
   
    return 0;
}

