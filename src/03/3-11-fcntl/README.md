 ## what is fcntl

 fcntl函数可以改变已经打开文件属性。    
 本例子中，fcntl(fd, F_GETFL, 0) , `F_GETFL`表示获取文件状态标志，更多参阅`fcntl(2)`

 ```
./main.out 0 < `tty`
read only

./main.out 0 < /dev/tty
read only

./main.out 1 > /dev/tty
write only

./main.out 1 <> /dev/tty
read write, append

./main.out 2 < /dev/tty
read write, append

./main.out 5 5<>/dev/tty
read write

 ```

 shell命令，以上使用了标准输入、输出、错误的重定向，`./main.out 5 5<>/dev/tty`表示在文件描述符5上打开文件/dev/tty以供读、写     

 本例代码，使用了`atoi`获取输入的文件描述符，`atoi` 意为 `ascii to integer`，main函数获取的参数是字符串的数字，`atoi`将其转化为int类型。