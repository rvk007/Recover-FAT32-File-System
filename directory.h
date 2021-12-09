#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "fat32disk.h"

void getRootDirectoryEntries(int fd, BootEntry* disk);

#endif
