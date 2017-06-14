#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int readLimit(int fd, int start, int size){
    char readBuffer[size];
    if(lseek(fd, start, SEEK_SET) < 0){
        perror("error, lseek()");
        return -1;
    }

    if(read(fd, readBuffer, size) < 0){
        perror("error, read()");
        return -1;
    }

    readBuffer[size] = '\0';
    //printf("after lseek(fd, %d, SEEK_SET), read %d size: %s\n", start, size, readBuffer);
    printf("after lseek(fd, %d, SEEK_SET), read %d size: ", start, size);

    int i;
    for(i=0; i<size; i++){
        if(readBuffer[i])
            putc(readBuffer[i], stdout);
        else
            fputs("\\0", stdout);
    }
    putc('\n', stdout);
}

int writeLimit(int fd, int start, char * buffer, size){
    if(lseek(fd, start, SEEK_SET) < 0){
        perror("error, lseek()");
        return -1;
    }
    if(write(fd, buffer, size) < 0){
        perror("error, write()");
        return -1;
    }
    return 0;
}

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

    printf("after open(tmpFile, O_RDWR) read: %s\n", readBuffer);

    readLimit(fd, 0, size);
    readLimit(fd, 26, 10);
    readLimit(fd, 26, 20);

    writeLimit(fd, 1, "aaa", 3);
    readLimit(fd, 0, size);
    close(fd);


    fd = open(tmpFile, O_APPEND|O_RDWR);
    if(fd<0){
        perror("error, open()");
        exit(1);
    }
    if(write(fd, "ccc", 3) < 0){
        perror("error, write()");
    }
    readLimit(fd, 0, size+3);
    close(fd);



    fd = open(tmpFile, O_RDWR);
    if(fd<0){
        perror("error, open()");
        exit(1);
    }
    writeLimit(fd, 1, "aaa", 3);
    readLimit(fd, 0, size);
    writeLimit(fd, 100, "bbb", 3);
    readLimit(fd, 0, 103); // hole file, there are '\0' where no data
    close(fd);

    exit(0);
}