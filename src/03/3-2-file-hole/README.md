## intro

本例子创建两个大小一样的文件对比，理解空洞文件

- 一个是空洞文件，使用lseek函数改变偏移量，然后写入数据
- 另一个不是，是重复填充`/0`

`od -c` 以字符方式打印文件内容，可以看见，文件`空洞`，未写入字节都被读作`\0`

> 对于没写过的字节位置，read函数读到的字节是`\0`

```
make && ./main.out
od -c file.hole.tmp
od -c file.nohole.tmp
```

```
0000000   B   e   f   o   r   e   H   o   l   e  \0  \0  \0  \0  \0  \0
0000020  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0
*
0040000   A   f   t   e   r   H   o   l   e  \0
0040012
```

`ls -ls`或`du -s`命令能看出两个`内容一样?`文件所占的字节块: 20与8

```
 8 -rw-r--r-- 1 xx xx 16394 Jun  6 18:49 file.hole.tmp
20 -rw-r--r-- 1 xx xx 16394 Jun  6 18:49 file.nohole.tmp
```

 > du命令, Linux上，一般报告是512字节块的块数，如果设置了环境变量POSIXLY_CORRECT，是1024字节块的块数。很多BSD类系统的是1024，Solaris的是512

## what is file hole

文件空洞是由所设置的偏移量超过文件尾端，并写入了某些数据后造成的。