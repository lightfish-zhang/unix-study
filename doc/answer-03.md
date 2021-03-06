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

### 假设一个进程执行下面3个函数调用：
```c
fd1 = open(path, oflags);
fd2 = dup(fd1);
fd3 = open(path, oflags);
```
画出类似于图3-9的结果图。对fcntl作用于fd1来说，F_SETFD命令会影响哪一个文件描述符？ F_SETFL呢？    

不画图了，简单来说，
- fd1和fd2共享同一个文件表项
- fd1,fd2,fd3在同一个进程表中

对fcntl的cmd参数
- `F_SETFD`命令设置`close-on-exec`标志，该标志以参数arg的`FD_CLOEXEC`位决定，应当了解很多现存的涉及文件描述符标志的程序并不使用常数 FD_CLOEXEC，而是将此标志设置为0(系统默认，在exec时不关闭)或1(在exec时关闭)

> fork子进程，子进程很可能会继续exec新的程序。子进程以写时复制（COW，Copy-On-Write）方式获得父进程的数据空间、堆和栈副本，这其中也包括文件描述符。刚刚fork成功时，父子进程中相同的文件描述符指向系统文件表中的同一项（这也意味着他们共享同一文件偏移量）。
> 有时我们fork子进程时已经不知道打开了多少个文件描述符（包括socket句柄等），这此时进行逐一清理确实有很大难度。我们期望的是能在fork子进程前打开某个文件句柄时就指定好：“这个句柄我在fork子进程后执行exec时就关闭”。即所谓的 close-on-exec。

[理解close-on-exec](http://blog.csdn.net/chrisniu1984/article/details/7050663)

```
#example
fcntl(fd, F_SETFD, FD_CLOEXEC);
# or
fcntl(fd, F_SETFD, 0);
```

- `F_SETFL`命令设置文件描述符标志，新标志值按第三个参数，如`fcntl(fd, F_SETFD, O_RDONLY)`，文件状态标志见书的图3-10

```
# example
int s = socket(PF_INET, SOCK_STREAM, 0);

fcntl(s, F_SETFL, O_NONBLOCK);  // 設定為非阻塞（non-blocking）
fcntl(s, F_SETFL, O_ASYNC);     // 設定為非同步 I/O
```

### 在Bourne shell, Bourne-again shell和Korn shell中，digit1>&digit2表示要将描述符digit1重定向至描述符digit2的同一文件。请说明下面命令的区别。

```
./a.out > outfile 2>&1
./a.out 2>&1 > outfile
```

一眼看上去两者好像没区别，实测后
- 第一条命令，stdout和stderr都会输出到outfile，打开outfile发现stderr都在stdout之前
- 第二条命令，stderr会输出到tty，stdout会输出到outfile

### 如果使用追加标志打开一个文件以便读写，能否仍用`lseek`在任一位置开始读？能否用`lseek`更新文件中任一部分的数据？请编写一段程序验证。

可以，注意的是:
- 仅仅使用一个标志，如`open(path, O_APPEND)`，只能`read`，需要两个标志才可以`write`，如`open(path, O_APPEND|O_RDWR)`，
- 使用`O_APPEND`且没有使用`lseek`修改偏移量，`open`后`write`时，是文件末尾写入，`open`后`read`，是从文件开头读

```c
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
```