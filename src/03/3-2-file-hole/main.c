#include "apue.h"
#include <fcntl.h>

char buf1[] = "BeforeHole";
char buf2[] = "AfterHole";

int main(void)
{
    int fd;

    if((fd = creat("file.hole.tmp", FILE_MODE)) < 0)
        err_sys("creat error");

    if(write(fd, buf1, 10) != 10)
        err_sys("buf1 write error");

    if(lseek(fd, 16384, SEEK_SET) == -1)
        err_sys("lseek error");

    if(write(fd, buf2, 10) != 10)
        err_sys("buf2 write error");


    int fd2;

    if((fd2 = creat("file.nohole.tmp", FILE_MODE)) < 0)
        err_sys("creat error");

    if(write(fd2, buf1, 10) != 10)
        err_sys("buf1 write error");

    for(int i=0; i<16374; i++){
        if(write(fd2, "\0", 1) != 1)
            err_sys("buf1 write error");
    }

    if(write(fd2, buf2, 10) != 10)
        err_sys("buf2 write error");

    exit(0);
}