#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "fat32disk.h"

typedef struct File {
    unsigned int size;                  // Number of sectors 
    unsigned int startCluster;          // Number of clusters
    char* name;       
} File;

typedef struct Directory {
    unsigned int size;                  // Number of sectors 
    unsigned int startCluster;          // Number of clusters
    char* name;                         // Name of the file or directory
    struct Directory** subDirectories;
    File** subFiles;
    unsigned int subDirCount;
    unsigned int subFileCount;
} Directory;

DirEntry initializeRootDirectory(DiskFileInfo *dFileInfo);

#endif
