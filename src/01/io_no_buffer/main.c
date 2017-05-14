//
// Created by lightfish on 5/14/17.
//
/*
    函数　open,read,write,lseek,close 提供了不带缓冲的I/O，这些函数都使用文件描述符
    如果愿意从标准输入读，并向标准输出写，以下程序可用于复制任一UNIX普通文件
    分配缓存buf，4096字节，循环读取文件描述符，read，直到EOF，如果是term下，可`Ctrl-d`
 */


#include "apue.h"
#define BUFFSIZE 4096

int main(void)
{
    int n;
    char buf[BUFFSIZE];

    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
        if(write(STDOUT_FILENO, buf, n) != n)
            err_sys("write error");

    if(n < 0)
        err_sys("read error");

    exit(0);
}