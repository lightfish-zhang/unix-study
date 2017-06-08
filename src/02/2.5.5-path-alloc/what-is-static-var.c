#include <unistd.h>
#include <stdio.h>

static long posix_version = 0;

void getPosixVersion(){
    if(0 == posix_version){
        printf("get posix_version\n");
        posix_version = sysconf(_POSIX_VERSION);        
    }
    printf("posix_version is %ld\n", posix_version);    
}

int main()
{
    getPosixVersion();
    getPosixVersion();
    return 0;
}

