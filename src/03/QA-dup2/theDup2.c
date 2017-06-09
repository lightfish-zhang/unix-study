#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int 
theDup2(int fd, int fd2)
{
    if(fd < 0 || fd2 <0){
        fprintf(stderr, "error: fd < 0 || fd2 < 0\n");
        return -1;
    }

    int fdMaxNum = sysconf(_SC_OPEN_MAX);
    if(fdMaxNum < 0){
        perror( "error: sysconf(_SC_OPEN_MAX)");
        return -1;
    }

    int fdList[fdMaxNum];
    int fdGet, i, j;

    for(i = 0; i<fdMaxNum; i++ ){
        fdGet = dup(fd);
        if(fdGet < 0){
            perror( "error: dup(fd)");
            return -1;
        }
        if(fdGet == fd2)
            break;
        fdList[i] = fdGet;
    }
    close(fd);

    for(j = i-1; j>0; j-- )
        close(fdList[j]);
    
    return (fd2 == fdGet) ? fdGet : -1;
}