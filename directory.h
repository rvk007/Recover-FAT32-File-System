#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdbool.h>
#include "fat32disk.h"

void getRootDirectoryEntries(int fd, BootEntry* disk);
DirEntry* getclusterPtr(char* file_content, BootEntry* disk, int cluster);
unsigned char* getDisk(int fd);
bool isDirectory(DirEntry* dirEntry);

#endif
