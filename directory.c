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

char* getDisk(unsigned int fd){
    struct stat fs;
    if(fstat(fd, &fs) == -1)
    {
        perror("Error while reading file stat");
    }
    return mmap(NULL , fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
}

DirEntry* getclusterPtr(char* file_content, BootEntry* disk, unsigned int cluster){
    unsigned int rootSector = (disk->BPB_RsvdSecCnt + disk->BPB_NumFATs*disk->BPB_FATSz32) + (cluster- 2)*disk->BPB_SecPerClus;
    unsigned int rootClusterOffset = rootSector*disk->BPB_BytsPerSec;

    DirEntry* dirEntry = (DirEntry*)(file_content + rootClusterOffset);
    if (dirEntry==NULL){
        perror("mmap failed");
        exit(1);
    }
    return dirEntry;
}

void showRootDirectory(DirEntry* dirEntry){
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
}

void getRootDirectoryEntries(int fd, BootEntry* disk){
    
    unsigned int nEntries = 0;
    unsigned int currCluster = disk->BPB_RootClus;
    unsigned int totalPossibleEntry = (disk->BPB_SecPerClus * disk->BPB_BytsPerSec)/sizeof(DirEntry);

    unsigned char *file_content = getDisk(fd);

    do{
        DirEntry* dirEntry = getclusterPtr(file_content,disk,currCluster);
        for(int m=0;m<totalPossibleEntry;m++){
            if (dirEntry->DIR_Attr == 0x00){        // no more dirEntry after this
                break;
            }

            if(dirEntry->DIR_Name[0] != 0xe5){      // do not show deleted files
                showRootDirectory(dirEntry);
                int startCluster = dirEntry->DIR_FstClusHI << 2 | dirEntry->DIR_FstClusLO;
                printf(" (size = %d, starting cluster = %d)\n",dirEntry->DIR_FileSize, startCluster);
                nEntries++;
            }
            dirEntry++;
        }
        unsigned int *fat = (unsigned int*)(file_content + disk->BPB_RsvdSecCnt*disk->BPB_BytsPerSec + 4*currCluster);
        if(*fat >= 0x0ffffff8 || *fat==0x00){
            break;
        }
        currCluster=*fat;
    } while(1);
    printf("Total number of entries = %d\n",nEntries);
}
