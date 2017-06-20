## 说明

执行步骤

```c
make
ls -l test.tmp
df ./
make && ./main.out
ls -l test.tmp
df ./
df ./
```

- 进程中，删除文件后，使用系统调用查看文件，发现文件不存在了。
- 但是，只要进程还未die，使用`df`查看该文件所在的目录，占用空间不变，然后进程die后，使用`df`查看目录占用空间减少了，即`i-node`的空间被释放。