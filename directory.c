#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>

#include "fat32disk.h"

bool isDirectory(DirEntry* dirEntry){
    return dirEntry->DIR_Attr == 0x10;
}

void getRootDirectoryEntries(int fd, BootEntry* disk){

    int rootSector = (disk->BPB_RsvdSecCnt + disk->BPB_NumFATs*disk->BPB_FATSz32) + (disk->BPB_RootClus - 2)*disk->BPB_SecPerClus;
    int rootClusterOffset = rootSector*disk->BPB_BytsPerSec;

    DirEntry* dirEntry = mmap(NULL, sizeof(DirEntry), PROT_READ, MAP_PRIVATE, fd, rootClusterOffset);
    if (dirEntry==NULL){
        perror("mmap failed");
        exit(1);
    }
    int nEntries = 0;
    while(dirEntry->DIR_Attr != 0x00){      // no more dirEntry after this
        if(dirEntry->DIR_Name[0] != 0xE5){      // do not show deleted files
            if (isDirectory(dirEntry)){
                int idx = 0;
                while(dirEntry->DIR_Name[idx]!=' '){
                    printf("%c",dirEntry->DIR_Name[idx]);
                    idx++;
                }
                printf("/");
            }
            else{
                for(int i=0;i<8;i++){
                    if(dirEntry->DIR_Name[i]==' ')
                        break;
                    printf("%c",dirEntry->DIR_Name[i]);
                }
                if(dirEntry->DIR_Name[8]!=' '){
                    printf(".");
                    for(int i=8;i<12;i++){
                        if(dirEntry->DIR_Name[i]==' ')
                            break;
                        printf("%c",dirEntry->DIR_Name[i]);
                    }
                }
            }
            printf(" (size = %d, starting cluster = %d)\n",dirEntry->DIR_FileSize, dirEntry->DIR_FstClusLO);
            nEntries++;
        }
        dirEntry++;
    }
    printf("Total number of entries = %d\n",nEntries);
}
