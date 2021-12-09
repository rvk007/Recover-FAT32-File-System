#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#include "fat32disk.h"

void *readFileSystem(const char *diskName){
    struct stat fs;

    int fd = open(diskName, O_RDONLY);
    if (fd<0){
        perror("Disk file could not be opened!");
        exit(1);
    }
    //printf("fd: %d",fd);

    if(fstat(fd, &fs)){
        perror("Disk file stat failed");
        exit(1);
    }
    //printf("size %d   \n",fs.st_size);
    void *file_content = mmap(NULL, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    //printf("file_content: %c\n",file_content[100]);
    if (file_content==NULL){
        perror("mmap failed");
        exit(1);
    }
    //printf("%c",file_content[2]);
    return file_content;
}

DiskFileInfo* updateBootEntry(void* start){
    DiskFileInfo *dFileInfo = (DiskFileInfo*)malloc(sizeof(DiskFileInfo));
    memset(dFileInfo, 0, sizeof(DiskFileInfo)); // malloc requires memset to allocate memory

    BootEntry* bootEntry = (BootEntry*)start;

    dFileInfo->start = start;       // start of the disk
    dFileInfo->n_fat = bootEntry->BPB_NumFATs;      //Number of fats
    dFileInfo->n_bytesPerSector = bootEntry->BPB_BytsPerSec;        
    dFileInfo->n_sectorPerCluster = bootEntry->BPB_SecPerClus;
    dFileInfo->n_reservedSectors = bootEntry->BPB_RsvdSecCnt;
    dFileInfo->rootCluster = bootEntry->BPB_RootClus;       // root cluster mostly 2
    dFileInfo->clusterOffsetInSectors = bootEntry->BPB_RsvdSecCnt + bootEntry->BPB_NumFATs*bootEntry->BPB_FATSz32; 
    // clusterOffsetInSectors: reserved sectors for boot sector + number of FATs * number of sectors in a FAT

    return dFileInfo;
}

