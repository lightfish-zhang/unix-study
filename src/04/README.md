## 文件的信息结构

```c

struct stat {
    mode_t  st_mode;    /* file type & mode (permissions), eg, 0600 */
    ino_t   st_ino;     /* i-node number (serial number) */
    dev_t   st_dev;     /* device number (file system) */
    dev_t   st_rdev;    /* device number for special files */
    nlink_t st_nlink;   /* number of links 链接计数 */
    uid_t   st_uid;
    git_t   st_git;
    off_t   st_size;
    struct timespec st_atime;
    struct timespec st_mtime;
    struct timespec st_ctime;
    blksize_t   st_blksize;
    blkcnt_t    st_blocks;
}

```

- `mode_t` 文件类型信息，一些宏的参数，见书P95
- `inode` [inode数据结构](https://zh.wikipedia.org/wiki/Inode)

## 命令
`ls` 常调用`stat`查看文件的信息结构

## unlink 与 st_nlink 与 i-node
- 一个文件指向一个i节点，每个i节点都有一个链接计数，当链接计数减少至0时且没有进程占用该文件，该文件占用的数据块可以释放，即删除。所以删除文件操作叫`unlink`。
- 重命名文件，链接计数不会改变。只需要构造新的指向i-node的新目录项，删除旧目录项。
- 硬链接会占用链接数，软连接不会。

- `/usr/include/linux/limits.h` 定义了链接数最大值和文件名最大值

```
#define LINK_MAX         127	/* # links a file may have */
#define NAME_MAX         255	/* # chars in a file name */
```