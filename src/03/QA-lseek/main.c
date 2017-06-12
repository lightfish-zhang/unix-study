#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
main()
{
    char * tmpFile = "test.tmp";
    int fd = open(tmpFile, O_RDWR|O_CREAT, 0644);
    if(fd<0){
        perror("error, open()");
        exit(1);
    }

    char writeBuffer[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    ssize_t size = write(fd, writeBuffer, sizeof(writeBuffer));
    printf("size is %ld\n", size);
    close(fd);

    fd = open(tmpFile, O_APPEND);
    if(fd<0){
        perror("error, open()");
        exit(1);
    }

    char readBuffer[size];
    if(read(fd, readBuffer, size) < 0){
        perror("error, read()");
        exit(1);
    }

    printf("read: %s\n", readBuffer);

    if(lseek(fd, 0, SEEK_SET) < 0){
        perror("error, lseek()");
        exit(1);
    }

    if(read(fd, readBuffer, size) < 0){
        perror("error, read()");
        exit(1);
    }

    printf("after lseek, read: %s\n", readBuffer);

    exit(0);
}