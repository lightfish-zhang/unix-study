#include "apue.h"
#include <fcntl.h>

int
main(int argc, char * argv[])
{
    if(argc != 2)
        err_quit("usage: a.out <pathname>");
    if(access(argv[1], R_OK) < 0)
        err_ret("read access error for %s", argv[1]);
    else
        printf("read access OK\n");

    if(access(argv[1], W_OK) < 0)
        err_ret("write access error for %s", argv[1]);
    else
        printf("write access OK\n");
    
    if(access(argv[1], X_OK) < 0)
        err_ret("excute access error for %s", argv[1]);
    else
        printf("excute access OK\n");

    if(open(argv[1], O_RDONLY) < 0)
        err_ret("open error for %s", argv[1]);
    else
        printf("open for reading OK\n");
    exit(0);
}