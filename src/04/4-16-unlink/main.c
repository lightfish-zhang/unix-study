#include "apue.h"
#include <fcntl.h>

int
main(void)
{
    if(creat("test.tmp", 0666) < 0)
        err_sys("creat error for file2.tmp");
    sleep(15);
    if(open("test.tmp", O_RDWR) < 0)
        err_sys("open error");
    if(unlink("test.tmp") < 0)
        err_sys("unlink error");
    printf("file unlinked\n");
    sleep(1);
    if(access("test.tmp", R_OK) < 0)
        err_ret("read access error for %s", "test.tmp");
    else
        printf("read access success\n");
    sleep(15);
    printf("done\n");
}