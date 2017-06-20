## 檔案預設權限：umask

umask 就是指定 『目前使用者在建立檔案或目錄時候的權限預設值』

- umask 的分數指的是『該預設值需要減掉的權限』因為 r、w、x 分別是 4、2、1 分
- umash 与 chmod 的分数时相反的

```
[root@study ~]# umask
0022
[root@study ~]# touch test1
[root@study ~]# mkdir test2
[root@study ~]# ll -d test*
-rw-r--r--. 1 root root 0  6月 16 01:11 test1
drwxr-xr-x. 2 root root 6  6月 16 01:11 test2
```

## 本例程序

执行结果

```
./main.out && ls -l *.tmp
-rw-rw-rw- 1 lightfish lightfish   0 6月  20 15:54 file1.tmp
-rw------- 1 lightfish lightfish   0 6月  20 15:54 file2.tmp
```

等效于以下shell命令

```
umask 0111
touch file1.tmp
umask 0177
touch file2.tmp
```