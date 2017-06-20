#include "apue.h"
#include <fcntl.h>

// st_mode屏蔽 用户读|用户写|用户执行|组、其他
#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int
main(void)
{
    umask(0);
    if(creat("file1.tmp", RWRWRW) < 0)
        err_sys("creat error for file1.tmp");
    umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(creat("file2.tmp", RWRWRW) < 0)
        err_sys("creat error for file2.tmp");
    return 0;
}