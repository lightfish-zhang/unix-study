#include <stdio.h>
#include <fcntl.h>

#define NAMESIZE 100

struct Item {
    short count;
    long total;
    char name[NAMESIZE];
};

void
err_sys(const char *errorMessage)
{
	fprintf(stderr, errorMessage);
    exit(1);
}

int main()
{
    int fd;

    if((fd = creat("file.tmp", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
        err_sys("creat error");

    FILE * fp;

    if((fp = fdopen(fd, "w"))<0)
        err_sys("fdopen error");

    struct Item * item;

    if(fwrite(&item, sizeof(struct Item), 1, fp) != 1)
        err_sys("fwrite error");

    return 0;
}

