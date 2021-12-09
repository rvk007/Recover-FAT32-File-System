#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

#include "fat32disk.h"

int getFileDirectory(const char *diskName){
    int fd = open(diskName, O_RDONLY);
    if (fd<0){
        perror("Disk file could not be opened!");
        exit(1);
    }
    return fd;
}

BootEntry* readFileSystem(int fd){

    BootEntry *disk = mmap(NULL, sizeof(BootEntry), PROT_READ, MAP_PRIVATE, fd, 0);
    if (disk==NULL){
        perror("mmap failed");
        exit(1);
    }
    return disk;
}

void showDiskInformation(BootEntry* disk){
        printf("Number of FATs = %d\n"
               "Number of bytes per sector = %d\n"
               "Number of sectors per cluster = %d\n"
               "Number of reserved sectors = %d\n",
               disk->BPB_NumFATs, disk->BPB_BytsPerSec, disk->BPB_SecPerClus, disk->BPB_RsvdSecCnt
        );
        fflush(stdout);
    }