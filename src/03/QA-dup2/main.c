#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "theDup2.c"

int
main()
{
    char * tmpFile = "/tmp/theDup2.tmp";
    char writeBuffer[] = "theDup2 run success!\n", readBuffer[80];

    int fd = open(tmpFile, (O_RDWR | O_CREAT), 0644);
    printf("fd is %d\n", fd);
    int fd2 = theDup2(fd, 100);
    if(fd2 < 0){
        fprintf(stderr,"error: theDup2\n");
        exit(1);
    }
    printf("fd2 is %d\n", fd2);
    if(write(fd2, writeBuffer, sizeof(writeBuffer)) < 0){
        perror("error: write");
        exit(1);
    }
    close(fd2);

    int fd3 = open(tmpFile, O_RDONLY);
    printf("fd3 is %d\n", fd3);
    int fd4 = theDup2(fd, 200);
    printf("fd4 is %d\n", fd4);
    if(fd4 < 0){
        fprintf(stderr,"error: theDup2\n");
        exit(1);
    }
    read(fd4, readBuffer, sizeof(readBuffer));
    printf("%s", readBuffer);
    close(fd4);

    if( remove(tmpFile) != 0 )
        perror("remove");

    exit(0);
}