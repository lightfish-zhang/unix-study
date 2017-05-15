## 时间值

````
cd /usr/include
time -p grep _POSIX_SOURCE */*.h > /dev/null
````

在ubuntu 16.4 下, time 加 -p 会报错

````
cd /usr/include
time grep _POSIX_SOURCE */*.h > /dev/null
> 0.01s user 0.03s system 1% cpu 2.502 total
````