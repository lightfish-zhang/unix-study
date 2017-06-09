## 第三章习题

### 当读写磁盘文件时，本章中描述的函数确实是不带缓冲机制的吗？

- 系统调用`read`和`write`都会经过内核的缓冲区高速缓存，
- 如果是getc putc(读写一个字节)，缓存为int buf，使用fgetc fputc(读写一行)，缓存为char buf[MAXLINE],但是它们最终都会调用`read`和`write`，依旧需要走内核的缓存区。
- `write`一般把数据复制到缓冲区，排入队列，延迟写入物理介质。通常，系统守护进程`update`会定期的`sync`，冲洗内核的块缓冲区。用户可以调用`sync`其立即进行。
- 像数据库的应用程序需要调用`fsync`等待磁盘操作结束返回以确保数据写入物理介质。
- 使用`fcntl`设置文件状态标志`O_SYNC`，可以使`write`一直等到数据写入物理介质才返回。
- 本题目的答案，第三章的讲述的函数都是带有缓冲机制的，没有哪个函数是真正意义上的不经过缓存区直接写入物理介质。

### 编写一个与3.12节中dup2功能相同的函数，要求不调用fcntl函数，并且要有正确的出错处理

思路： 笨方法，使用`dup(2)`重复执行，直到返回的`fd`为需要的值，最大执行次数由`sysconf(_SC_OPEN_MAX)`获得。该函数非原子操作。

```c

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int 
theDup2(int fd, int fd2)
{
    if(fd < 0 || fd2 <0){
        fprintf(stderr, "error: fd < 0 || fd2 < 0");
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

```