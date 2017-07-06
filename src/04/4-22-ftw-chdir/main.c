#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <sys/time.h>

typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *k, Myfunc *);
static int dopath(Myfunc *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


int main(int argc, char *argv[])
{
    struct timeval start, end;
    int ret;
    int timeuse;
    if (argc != 2)
        err_quit("usage: ftw <starting-pathname>");
    gettimeofday( &start, NULL );
    ret = myftw(argv[1], myfunc);
    gettimeofday( &end, NULL );

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;

    if (ntot == 0)
        ntot = 1;

    printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);

    printf("directories = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);

    printf("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);

    printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);

    printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);

    printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);

    printf("sockets = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);

    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec;
    printf("time: %d us\n", timeuse);
    exit(ret);
}

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

static char filePath[10000]; // set it long not more
static size_t pathlen;
static int myftw(char *pathname, Myfunc *func)
{
    strcpy(filePath, pathname);
    return(dopath(func));
}

static int dopath(Myfunc *func)
{
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;
    char *tmp;

    if (lstat(filePath, &statbuf) < 0)
        return(func(filePath, &statbuf, FTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0)
        return(func(filePath, &statbuf, FTW_F));

    if ((ret = func(filePath, &statbuf, FTW_D)) != 0)
        return(ret);


    if (chdir(filePath) < 0) 
        err_quit("chdir error");

    if ((dp = opendir(".")) == NULL)
        return(func(filePath, &statbuf, FTW_DNR));

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;
        

        strcpy(filePath, dirp->d_name);
        //printf("%s\n",filePath);
        if ((ret = dopath(func)) != 0)
            break;
    }

    if (closedir(dp) < 0)
        err_ret("can't close directory %s", filePath);

    if (chdir("..") < 0) 
        err_quit("chdir to .. error");
    return(ret);
}


               
static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
    switch (type) {
    case FTW_F:
        switch (statptr->st_mode & S_IFMT) {
        case S_IFREG: nreg++; break;
        case S_IFBLK: nblk++; break;
        case S_IFCHR: nchr++; break;
        case S_IFIFO: nfifo++; break;
        case S_IFLNK: nslink++; break;
        case S_IFSOCK: nsock++; break;
        case S_IFDIR:
            err_dump("for S_IFDIR for %s", pathname);
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        err_ret("can't read directory %s", pathname);
    case FTW_NS:
        err_ret("stat error for %s", pathname);
        break;
    default:
        err_dump("unknown type %d for pathname %s", type, pathname);
    }
    return(0);
}