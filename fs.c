#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libdisk.h"
#include "fs.h"

#define MAXFILES 64
#define BLOCKSPERDISK 40
#define INODE 2

//todo: make dynamic table for open files, not static arr

//for inodes: 4th and 5th bytes is size, 6th byte starts the file name

//free index at 16

//staring at index 16 is the block number of the files belonging to that inode

typedef struct {
    int inode_block;
    int offset;
    char name[MAXFILENAME];
    int isOpen;
    int size;
} file_t;

file_t files[MAXFILES]; //indexed by file descriptor (fd made by this process, not a real fd)



int mounted_diskNum;

int fd_num = -1;

//need dynamic array to store shit???
FILE* mounted = NULL;
int tfs_mkfs(char *filename, int nBytes){

    /////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////

    //int blocks = nBytes/BLOCKSIZE;
    int diskNum = openDisk(filename, nBytes);
    if (diskNum < 0){
        printf("tfs_mkfs fail\n");
        return -1;
    }
    char superblock[BLOCKSIZE]; //need to encode where the free blocks are in the superblock
    //probably do indexed
    uint8_t type = 1;
    uint8_t magicNum = 0x44;
    uint64_t free_blocks = 0xFFFFFFFFFE; //all blocks are free at the start except for superblock
    memcpy(superblock, &type, 1);
    memcpy(&superblock[1], &magicNum, 1);
    memcpy(&superblock[4], &free_blocks, 5);
    

    

    //start at superblock[4] for data
    
    
    
   
    mounted = getfp(filename);
    if (!mounted){
        printf("failed to get fp in mkfs\n");
    }

    fseek(mounted, 0, SEEK_SET);
    writeBlock(diskNum, 0, superblock);
    

    return diskNum;

}
int tfs_mount(char *diskname){
    mounted_diskNum = getDiskNum(diskname);
    if (mounted_diskNum < 0){
        printf("disk does not exist in mount\n");
        return -1;
    }
    FILE *f = getfp(diskname);
    if (!f){
        printf("failed to get fp in mount\n");
        return -1;
    }
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
  

    //go through all the inodes and see if there is a file with such a name already
    uint8_t block[BLOCKSIZE];
    int diskSize = getSize(mounted_diskNum);
    for (int i = 1; i < diskSize; i++){
        int res = readBlock(mounted_diskNum, i, (void*)block);
        if (res < 0){
            printf("read error in open file\n");
            return -1;
        }

        if (block[0] == INODE){
            char curr_filename[MAXFILENAME];
            strcpy(curr_filename, (char *)&block[5]);
            if (strcmp(curr_filename, name) == 0){
                //there is already a file with this name
                for (int j = 0; j < MAXFILES; j++){
                    if (files[j].inode_block == i){ //this is the inode of the file trying to be opened
                        return j; //this is the fd of the file
                    }   
                }
                

            }
        }
        
    }
    printf("did not find file w/ name %s\n", name);
    //there is no file with this name 
    //create an inode for this file 
    uint8_t inode[BLOCKSIZE];
    memset(inode, 0, BLOCKSIZE);
    uint8_t type = 2;
    uint8_t magicNum = 0x44;
    uint16_t size = 0;
    
    memcpy(inode, &type, 1);
    memcpy(&inode[1], &magicNum, 1);
    memcpy(&inode[4], &size, 2);

    if ((strlen(name) + 1) >= MAXFILENAME) {
        printf("filename too long\n");
        return -1;
    }
    memcpy(&inode[6], name, strlen(name)); //strlen(name) should not be more than 8

    //NEED TO GET NEXT AVAILABLE BLOCK
    uint8_t superblock[BLOCKSIZE];
    
    int res = readBlock(mounted_diskNum, 0, (void*) superblock);
    if (res < 0){
        printf("failed to read block A\n");
        return -1;
    }

    uint64_t positions = 0;

    memcpy(&positions, &superblock[4], 5); //get the field which speciefies open blocks
    
    int inodeBlock = find_opening(positions);
    if (inodeBlock < 0){
        printf("disk is full\n");
        return -1;
    }
    set_closed(inodeBlock);
    res = writeBlock(mounted_diskNum, inodeBlock, (void*)inode); //plus one because the first block is always the superblock
    if (res < 0){
        printf("write block error in open file\n");
        return -1;
    }
   
    file_t newFile;
    newFile.inode_block = inodeBlock;
    newFile.offset = 0;
    newFile.isOpen = 1;
    newFile.size = 0;
    strcpy(newFile.name, name); //name should be max 8 chars w/ out null
    fd_num++;
    files[fd_num] = newFile;
    return fd_num;

    
} 

int find_opening(uint64_t value){
    for (int i = 0; i < 40; i++) {
        if (((value >> i) & 1) == 1) {
            return i; //because first block is always superblock
        }
    }
    return -1; // there are no open blocks in the drive
}

int set_closed(int bNum){ //sets a block as closed in the superblock
    uint8_t superblock[BLOCKSIZE];
    readBlock(mounted_diskNum, 0, (void*) superblock);
    uint64_t positions = 0;
    memcpy(&positions, &superblock[4], 5);
    positions &=  ~(1ULL << bNum);

    memcpy(&superblock[4], &positions, 5);
    int res = writeBlock(mounted_diskNum, 0, (void*) superblock);
    if (res < 0){
        printf("write error in set closed\n");
        return -1;
    }
    return 0;

}
int set_open(int bNum){
     uint8_t superblock[BLOCKSIZE];
    readBlock(mounted_diskNum, 0, (void*) superblock);
    uint64_t positions = 0;
    memcpy(&positions, &superblock[4], 5);
    positions |=  (1ULL << bNum);

    memcpy(&superblock[4], &positions, 5);
    int res = writeBlock(mounted_diskNum, 0, (void*) superblock);
    if (res < 0){
        printf("write error in set closed\n");
        return -1;
    }
    return 0;
}

int tfs_closeFile(int fd){
    files[fd].isOpen = 0;
    return 0;
}

int tfs_writeFile(int fd, char *buffer, int size){
    if (!(files[fd].isOpen)){
        printf("trying to write to a file that is not open\n");
        return -1;
    }
    uint8_t magicnum = 0x44;
    if (size > 255*40){
        printf("size too large\n");
    }

    uint8_t superblock[BLOCKSIZE];
    int res = readBlock(mounted_diskNum, 0, (void*) superblock);
    uint64_t positions = 0;
    memcpy(&positions, &superblock[4], 5); //get field that specifies which blocks are open
    

    
    if (res < 0){
        printf("error reading superblock in write\n");
        return -1;
    }
    int inodeBlockNum = files[fd].inode_block;
    uint8_t inodeBlock[BLOCKSIZE];
    res = readBlock(mounted_diskNum, inodeBlockNum, (void*) inodeBlock); //read the inode block, will need to modify it
    if (res < 0){
        printf("error reading inode in write\n");
        return -1;
    }
    uint16_t prev_size = 0;
    memcpy(&prev_size, &inodeBlock[4], 2);
    if (prev_size != 0){
        //need to truncate file before writing to it
        for (int i = 0; i < BLOCKSIZE; i++){
            uint8_t blockNumToDelete;
            memcpy(&blockNumToDelete, &inodeBlock[16+i], 1);
            if (blockNumToDelete == 0){
                break;
            }
            set_open(blockNumToDelete);
            positions |=  (1ULL << blockNumToDelete);
            uint8_t resetBlock[BLOCKSIZE];
            memset(resetBlock, 0, BLOCKSIZE);
            res = writeBlock(mounted_diskNum, blockNumToDelete, (void*) resetBlock);
            memcpy(&inodeBlock[16+i], resetBlock, 1); //delete this file from inode indexing
        }
    }

    //if prev size is 0 (no truncation needs to be done)
    memcpy(&inodeBlock[4], (uint16_t*)&size, 2); //copy the size into the inode
    int numBlocks = (size / (BLOCKSIZE - 4)) + 1; //only have blocksize - 4 bytes per block for DATA, 4 bytes are used for other
    for (int i = 0; i < numBlocks; i++){
        int fileBlockNum = find_opening(positions);
        if (fileBlockNum < 0){
            printf("disk is full in write\n");
            return -1;
        }
        set_closed(fileBlockNum); //rewrite superblock to count this block as not available
        positions &=  ~(1ULL << fileBlockNum); //for local copy of positions

        memcpy(&inodeBlock[16+i], (uint8_t*)&fileBlockNum, 1); //set this block num in the inode indexing
        //create the file
        uint8_t fileBlock[BLOCKSIZE];
        memset(fileBlock, 0, BLOCKSIZE);
        uint8_t type = 3; //for file
        memcpy(fileBlock, &type, 1);
        memcpy(&fileBlock[1], &magicnum, 1);

        uint8_t data[BLOCKSIZE - 4];
        //memcpy(data, &buffer[(BLOCKSIZE - 4) * i], BLOCKSIZE - 4); //get the data
        if (i == (numBlocks - 1)){
            //this is the last block that needs to be written
            int dataSize = size - (BLOCKSIZE - 4) * i;
            memcpy(data, &buffer[(BLOCKSIZE - 4) * i], dataSize); //get the data
            memcpy(&fileBlock[4], data, dataSize);
            res = writeBlock(mounted_diskNum, fileBlockNum, (void*) fileBlock);
            if (res < 0){
                printf("write error in write\n");
                return -1;
            }
        }
        else{ //writing full 254 bytes
            memcpy(data, &buffer[(BLOCKSIZE - 4) * i], BLOCKSIZE - 4); //get the data
            memcpy(&fileBlock[4], data, (BLOCKSIZE - 4));
            res = writeBlock(mounted_diskNum, fileBlockNum, (void*) fileBlock);
            if (res < 0){
                printf("write error in write\n");
                return -1;
            }

        }
        
        


    }
    //need to rewrite the inode (inode buffer already has correct size)
    files[fd].size = size;
    files[fd].offset = 0;
    res = writeBlock(mounted_diskNum, inodeBlockNum, (void*) inodeBlock);

    return 0;
    

    


}

int tfs_deleteFile(int fd){
    int inodeBlockNum = files[fd].inode_block;
    uint8_t inodeBlock[BLOCKSIZE];
    int res = readBlock(mounted_diskNum, inodeBlockNum, (void*) inodeBlock);
    if (res < 0){
        printf("failed to read inode in delete file\n");
        return -1;
    }

    uint8_t resetBlock[BLOCKSIZE];
    memset(resetBlock, 0, BLOCKSIZE);
    for (int i = 0; i < BLOCKSIZE - 16; i++){
        uint8_t blockNumToDelete;
        memcpy(&blockNumToDelete, &inodeBlock[16+i], 1);
        if (blockNumToDelete == 0){
            break;
        }
        set_open(blockNumToDelete);
        //positions |=  (1ULL << blockNumToDelete);
        
        res = writeBlock(mounted_diskNum, blockNumToDelete, (void*) resetBlock);
        
    }

    set_open(inodeBlockNum);
    res = writeBlock(mounted_diskNum, inodeBlockNum, (void*) resetBlock);

    return 0;

}