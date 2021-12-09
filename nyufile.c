#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fat32disk.h"
#include "directory.h"

int main(int argc, char **argv){
    int flag=0, anyOption=0;

    // get file system information
    void *start = readFileSystem(argv[optind]);
    DiskFileInfo *df = updateBootEntry(start);
    void showDiskInformation(){
        printf("Number of FATs = %d\n"
               "Number of bytes per sector = %d\n"
               "Number of sectors per cluster = %d\n"
               "Number of reserved sectors = %d\n",
               df->n_fat, df->n_bytesPerSector, df->n_sectorPerCluster, df->n_reservedSectors
        );
        fflush(stdout);
    }
    
    // go through all of the flags
    while((flag=getopt(argc, argv, "ilr:R:s:")) != -1){
        anyOption=1;
        switch(flag){
            case 'i':
                showDiskInformation();
                break;
            case 'l':
                printf("case l");
                initializeRootDirectory(df);
                // showRootDirectory(strt)
                break;
            case 'r':
                printf("case r");
                break;
            case 'R':
                printf("case R");
                break;
            case 's':
                printf("case s");
                break;
            default:
                exit(1);
        }
    }

    // if no flag is given show the usage and exit
    if (anyOption==0){
        printf("Usage: ./nyufile disk <options>\n"
                "   -i                     Print the file system information.\n"
                "   -l                     List the root directory.\n"
                "   -r filename [-s sha1]  Recover a contiguous file.\n"
                "   -R filename -s sha1    Recover a possibly non-contiguous file.\n")
        ;
        fflush(stdout);
        exit(1);
    }
}