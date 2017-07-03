## 第四章习题

### 1.如果参数pathname为符号链接，state与lstat的区别是什么？

当命名的文件是一个符号链接时，`lstat`返回该符号链接的有关信息，`stat`返回该符号链接引用的文件的信息

### 2.如果文件模式创建屏蔽字为777(八进制)，结果会怎样？用shell的umask命令验证该结果。

执行`umask 0777`命令后，创建文件，例如`touch file.tmp`，可见

```
---------- 1 lightfish lightfish   0 7月   2 22:46  file.tmp
```

之后使用`vim`或者`cat`命令，非超级用户会遇到权限不足的提示，超级用户才可读写

### 3.关闭一个你所拥有文件的用户读权限，将导致拒绝你访问自己的文件，对此进行验证

```
➜  /tmp touch file.tmp
➜  /tmp ll file.tmp 
-rw-r--r-- 1 lightfish lightfish 0 7月   2 22:50 file.tmp
➜  /tmp sudo chmod 0244 file.tmp 
➜  /tmp ll file.tmp 
--w-r--r-- 1 lightfish lightfish 0 7月   2 22:50 file.tmp
➜  /tmp cat file.tmp 
cat: file.tmp: Permission denied
```

### 4.创建文件foo和bar后，运行图4-9的程序，将发生什么？

没影响

### 5. 4.12节中讲到一个普通文件的大小可以为０，同时我们有知道`st_size`字段是为目录或符号链接定义的，那么目录和符号链接的长度是否可以为０？

目录和符号链接的长度不为０

- 目录文件包含了其他文件的名字与指向这些文件相关的信息的指针（某些unix下为inode号码），也就是下属的文件的列表，空目录文件中，至少存在两条记录，`.`与`..`以及它们的inode号码
- 符号链接，shell下使用`ln -s`生成的软链接文件，符号链接对指向何种对象没有文件系统的限制，比起硬链接，它可以指向不同的文件系统，也不需超级用户权限，可以知道，指向的对象的信息保存在符号链接文件内容中，不可能为０。

### 6. 编写一个类似`cp(1)`的程序，它复制包含空洞的文件，但不将字节0写到输出文件中去

- 原因，常规复制一个空洞文件，源文件和复制本的大小会不一样。因为复制过程中，先读源文件，空洞部分读`\0`字节，读出来并写入复制本。空洞文件的空洞不占空间，而复制本的`\0`字节占空间。
- 实现思路，与制造空洞文件的方式一样，在写入复制本文件时，当写入`\0`字节时，干脆使用`lseek`修改文件的偏移量。

代码待补。。。

### 7. 在4.12节ls命令的输出中，core和core.copy的访问权限不同，如果创建两个文件时umask没有变，说明为什么会发生这种差别

- 本人在自己机器上没能还原这个现象，可能是实现的环境不一样。
- 分析下4.12节的复制过程，如果创建两个文件时umask一样，（问题原文是`没有变`，应该是使用默认的权限位），4.12节是使用`cat`和shell的`重定向`标准输出到一个文件，实现的复制，有可能是shell环境的差异所致吧，如果是用`cp -a`的shell命令来复制，两个文件的访问权限应该保证是一样的。

### 8. 在运行图4-16的程序时，使用了df(1)命令来检查空闲的磁盘空间。为什么不使用du(1)命令?

- df命令用于查看目前所有文件系统的最大可用空间及使用情形，也就是可以查看空闲磁盘块的数量或已使用磁盘块的数量，访问的是文件系统的空闲磁盘块列表。
- du命令用于查看目录下所有子目录文件的占用空间总计，它是遍历所有子目录文件，一般使用`--max-depth=`限制遍历深度，这里的遍历是，查看目录文件，获取文件列表，访问每一个文件对应的inode节点获取所占磁盘块的数量，统计占用空间。建议使用`-h`参数获取`Human-readable`的结果。
- 在图4-16实例中，是为了实践检验`进程未死亡前，进程删除文件时对应的inode的链接数为0，系统并不会立即回收磁盘块，只有当进程关闭该文件或者终止，内核关闭该进程打开的文件时才会检查inode的链接数是否为0，从而回收磁盘块。`这过程。
-  根据实际情况分析，df与du的区别是目标对象不同，`文件系统`与`目录文件`，更深层次的是，`文件系统的空闲磁盘块列表的总计`与`目录文件的文件列表的inode的磁盘块列表的总计`。
- 4-16实例中，删除文件，也就是将文件从目录文件移除，且修改inode的链接数-1
- 综上所知，在4-16实例中，我们需要使用工具观察当文件从目录文件的文件列表移除且inode的链接数减为0时磁盘块是否回收，`du`命令不符合要求，`df`符合。


### 9. 图4-20中显示unlink函数会修改文件状态更改时间，这是怎样发生的？

- `unlink`系统调用，过程是:将文件从目录文件的文件列表中移除，且修改该文件的inode的链接数-1。修改了`父目录文件`与`文件指向的inode`
- 三个时间段解释
    + `st_atim` 文件数据的最后访问时间
    + `st_mtim` 文件数据的最后修改时间
    + `st_ctim` i节点状态的最后更改时间

- 如4-20图显示，unlink会修改引用的文件或目录的`st_ctim`，修改其父目录的`st_mtim`与`st_ctim`
- 综上所述，修改了父目录的内容，影响父目录的文件大小，所以更新了父目录的`st_mtim`与`st_ctim`。将文件的inode的链接数-1，所以更新了本文件的`st_ctim`。

### 10. 4.22节中，系统对可打开文件数的限制对myftw函数会产生什么影响。