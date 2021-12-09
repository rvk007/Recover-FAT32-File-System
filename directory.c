#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "fat32disk.h"
#include "directory.h"


bool isDirectory(DirEntry* dir){
    // check if a a child is directory or file
    if (dir->DIR_Name[0] == '.')
        return true;        //FIXME: I think it should be false given line 1 in getRootDirectoryEntries ??
    return dir->DIR_Attr & 0x10;        //FIXME:0x10 represents a directory // how does it work ??
}

unsigned int readDisk(void* fatStart, unsigned int cluster){
    unsigned int* ptr = (int *)(fatStart + sizeof(unsigned int) * cluster);
    return *ptr;
}

unsigned int clusterSectors(DiskFileInfo *dFileInfo, unsigned int cluster){
    // return number of sectors from starting of the boot sector to starting of the current cluster
    if (cluster<2){
        perror("Cluster can not be less than 2");
        exit(1);
    }
    return dFileInfo->clusterOffsetInSectors + (cluster - dFileInfo->rootCluster)*dFileInfo->n_sectorPerCluster;
}

void* sectorPtr(DiskFileInfo *dFileInfo, unsigned int currSector){
    // return the pointer of current sector
    // from start of the disk file, move of current number of cluster * number of bytes per sectors
    return (void*)dFileInfo->start + currSector*dFileInfo->n_bytesPerSector; 
}

unsigned int getNextCluster(DiskFileInfo *dFileInfo, unsigned int cluster){
    unsigned int nextCluster = 0;
    void* fatStart = sectorPtr(dFileInfo, dFileInfo->n_reservedSectors);
    nextCluster = readDisk(fatStart, cluster);
    if (nextCluster < 0x002 || nextCluster >= 0x0FFFFFF0)
        return 0;
    return nextCluster;
}

Directory* createDirectory(DiskFileInfo *dFileInfo, DirEntry* dirEntry){
    Directory* dir = (Directory*)malloc(sizeof(Directory));
    memset(dir, 0, sizeof(Directory));
    
    dir->name = dirEntry->DIR_Name;
    dir->size = 0;      // byte size of a directory is 0
    dir->startCluster = dFileInfo->rootCluster;
    dir->subDirCount=0;
    dir->subFileCount=0;
    dir->subDirectories = malloc(100*sizeof(Directory)); //FIXME: change 100 to a variable
    dir->subFiles = malloc(100*sizeof(File)); //FIXME: change 100 to a variable
    return dir;
}

File* createFile(DiskFileInfo *dFileInfo, DirEntry* dirEntry){
    File* file = (File*)malloc(sizeof(File));
    memset(file, 0, sizeof(File));

    file->name = dirEntry->DIR_Name;
    file->size = dirEntry->DIR_FileSize;
    file->startCluster = dFileInfo->rootCluster;
    return file;
}

void getRootDirectoryEntries(Directory* dir, DiskFileInfo *dFileInfo){
    // loop thorough all the clusters and store information about each directory and file in the root directry
    if (dir->name[0] == '.') return; // 1 need more info about '.' ??

    unsigned int cluster = dir->startCluster;
    int nEntries = (dFileInfo->n_sectorPerCluster * dFileInfo->n_bytesPerSector) / sizeof(DirEntry);
    
    do{
        
        void* ptrSector = sectorPtr(dFileInfo, clusterSectors(dFileInfo, cluster));
        for(int i=0; i<nEntries; i++){
            DirEntry* childDir = (DirEntry*)(ptrSector + i * sizeof(DirEntry));
            
            if (childDir->DIR_Name[0] == 0x00) break; // same as 1 ??
            if (childDir->DIR_Name[0] == 0xE5) continue; // file or directory is already deleted; do not report this

            if(isDirectory(childDir)){
                Directory* subDir = createDirectory(dFileInfo, childDir);
                dir->subDirectories[dir->subDirCount++] = subDir;
            }
            else{
                File* subFile = createFile(dFileInfo, childDir);
                dir->subFiles[dir->subFileCount++] = subFile;
            }
        }
        cluster = getNextCluster(dFileInfo, cluster);
    }while(cluster!=0);
}

DirEntry initializeRootDirectory(DiskFileInfo *dFileInfo){
    DirEntry* dirEntry = (DirEntry*)malloc(sizeof(DirEntry));
    // memset(dirEntry->DIR_Name,' ',11);
    // memcpy(dirEntry->DIR_Name,"fat32",5);
    
    Directory* dir = createDirectory(dFileInfo, dirEntry); // returns root directory
    getRootDirectoryEntries(dir, dFileInfo);
}
