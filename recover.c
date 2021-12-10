#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "fat32disk.h"
#include "directory.h"
#define print(d) printf("%d\n",d)
#define prints(s) printf("%s\n",s)

unsigned char* getfilename(DirEntry* dirEntry){
    unsigned char *ptrFile = malloc(11);
    unsigned int idx=0;
    for(int i=0;i<8;i++){
        if(dirEntry->DIR_Name[i]==' ')
            break;
        ptrFile[idx] = dirEntry->DIR_Name[i];
        idx++;
    }
    if(dirEntry->DIR_Name[8]!=' '){
        ptrFile[idx] = '.';
        idx++;
        for(int i=8;i<12;i++){
            if(dirEntry->DIR_Name[i]==' ')
                break;
            ptrFile[idx] = dirEntry->DIR_Name[i];
            idx++;
        }
    }
    return ptrFile;
}

void unmapDisk(unsigned char* file_content, int fileSize){
    munmap(file_content, fileSize);
}

void updateRootDir(unsigned char* file_content , BootEntry* disk, char *filename, int nEntries){
    unsigned int rootSector = (disk->BPB_RsvdSecCnt + disk->BPB_NumFATs * disk->BPB_FATSz32);
    unsigned int rootClusterOffset = rootSector*disk->BPB_BytsPerSec;

    file_content[rootClusterOffset + nEntries] = (unsigned char) filename[0];
    
}

void updateFat(unsigned char* file_content , BootEntry* disk, int currCluster, unsigned int value){
    // update fat 1
    unsigned int *fat1 = (unsigned int*)(file_content + disk->BPB_RsvdSecCnt * disk->BPB_BytsPerSec + 4*currCluster);
    *fat1 = value;
    // update fat 2
    unsigned int *fat2 = (unsigned int*)(file_content + (disk->BPB_RsvdSecCnt + disk->BPB_FATSz32) * disk->BPB_BytsPerSec  + 4*currCluster);
    *fat2 = value;
}

int getDeletedDirEntry(int fd, BootEntry* disk, char *filename){
    unsigned int nEntries=0;
    unsigned int rootSector = (disk->BPB_RsvdSecCnt + disk->BPB_NumFATs*disk->BPB_FATSz32);
    unsigned int rootClusterOffset = rootSector * disk->BPB_BytsPerSec;
    unsigned int currCluster = disk->BPB_RootClus;
    unsigned int totalPossibleEntry = (disk->BPB_SecPerClus * disk->BPB_BytsPerSec)/sizeof(DirEntry);

    struct stat fs;
    if(fstat(fd, &fs) == -1)
    {
        perror("Error while reading file stat");
    }
    unsigned char* file_content  = mmap(NULL , fs.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    int fileCount=0;
    // store first occurence
    int nEntries1=0;
    DirEntry* dirEntry1;
    
    do{
        DirEntry* dirEntry = getclusterPtr(file_content,disk,currCluster);
        for(int m=0;m<totalPossibleEntry;m++){
            if (dirEntry->DIR_Attr == 0x00){        // no more dirEntry after this
                break;
            }

            if(isDirectory(dirEntry)==0 && dirEntry->DIR_Name[0] == 0xe5){      // do not show deleted files
                if (dirEntry->DIR_Name[1]==filename[1]){
                    char *recFilename = getfilename(dirEntry);
                    if(strcmp(recFilename+1,filename+1)==0){

                        // more than one file with same name except first character
                        if (fileCount<1){
                            nEntries1=nEntries;
                            dirEntry1=dirEntry;
                            fileCount++;
                        }
                        else{
                            printf("%s: multiple candidates found\n",filename);
                            fflush(stdout);
                            return 1;
                        }
                    }
                }
            }
            dirEntry++;
            nEntries+=sizeof(DirEntry);
        }
        unsigned int *fat = (unsigned int*)(file_content + disk->BPB_RsvdSecCnt*disk->BPB_BytsPerSec + 4*currCluster);
        if(*fat >= 0x0ffffff8 || *fat==0x00){
            break;
        }
        currCluster=*fat;
    } while(1);

    if (fileCount==1){
        updateRootDir(file_content, disk, filename, nEntries1);

        if (dirEntry1->DIR_FileSize > disk->BPB_SecPerClus * disk->BPB_BytsPerSec){
            int fileSize = dirEntry1->DIR_FileSize;
            int nBytesPerCluster = disk->BPB_SecPerClus * disk->BPB_BytsPerSec;
            int n_Clusters = 0;
            int startCluster = dirEntry1->DIR_FstClusHI << 2 | dirEntry1->DIR_FstClusLO;

            if (fileSize % nBytesPerCluster)
                n_Clusters = fileSize/nBytesPerCluster + 1;
            else
                n_Clusters = fileSize/nBytesPerCluster;
            
            for(int i=1;i<n_Clusters;i++){
                updateFat(file_content , disk, startCluster, startCluster+1);
                startCluster++;
            }
            updateFat(file_content , disk, startCluster, 0x0ffffff8);
        }
        else
            updateFat(file_content , disk, currCluster+1, 0x0ffffff8); // why +1 ?? but works
        unmapDisk(file_content, fs.st_size);
                        
        printf("%s: successfully recovered\n",filename);
        fflush(stdout);
        return 1;
    }
    return -1;
}

void recoverFile(int fd, BootEntry* disk, char *filename){
    if (getDeletedDirEntry(fd, disk, filename) == -1){
        printf("%s: file not found\n",filename);
        fflush(stdout);
    }
}