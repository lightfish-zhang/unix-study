## intro

本例子创建两个大小一样的文件，一个是空洞文件，另一个不是，`od -c` 以字符方式打印文件内容，可以看见，文件`空洞`，未写入字节都被读作`\0`

```
make && ./main.out
od -c file.hole.tmp
od -c file.nohole.tmp
```

以下命令能看出两个文件，一个占用了20个磁盘块，一个占用了8个磁盘块

```
ls -ls file.hole.tmp
ls -ls file.nohole.tmp
```