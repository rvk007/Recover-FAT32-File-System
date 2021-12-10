#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "fat32disk.h"
void recoverFile(int fd, BootEntry* disk, char *filename);
#endif
