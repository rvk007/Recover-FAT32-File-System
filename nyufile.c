#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "fat32disk.h"
#include "directory.h"
#include "recover.h"
#define print(d) printf("%d\n",d)

int main(int argc, char **argv){
    int flag=0, anyOption=0;
    int information=0, listRootDir=0, recoverRegFile=0;
    char *recOptarg;

    // get file system information
    int fd = getFileDirectory(argv[optind]);
    BootEntry* disk = readFileSystem(fd);

    // go through all of the flags
    while((flag=getopt(argc, argv, "ilr:R:s:")) != -1){
        anyOption=1;
        switch(flag){
            case 'i':
                information=1;
                break;
            case 'l':
                listRootDir=1;
                break;
            case 'r':
                recoverRegFile=1;
                recOptarg = optarg;
                break;
            case 'R':
                printf("case R");
                break;
            case 's':
                printf("case s");
                break;
            default:
                showUsage();
                break;
        }
    }
    
    if (anyOption==0)
        showUsage();

    if (information)
        showDiskInformation(disk);
    else if (listRootDir)
        getRootDirectoryEntries(fd, disk);
    else if(recoverRegFile){
        if (recOptarg == NULL)
            showUsage();
        recoverFile(fd, disk, recOptarg);
    }
}