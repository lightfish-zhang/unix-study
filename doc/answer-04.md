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

- 4-22节的`myftw`代码见附录
- 分析代码，`myftw()`中使用自定义的`dopath()`函数来遍历所有文件，且`dopath()`是递归方法，执行步骤如下
    + 静态（文件全局）变量数组`fullpath`用作临时保存当前处理的文件的路径，每一个元素是目录文件名或者其他文件名
    + 判断是否目录文件，如果是，`opendir()`获取目录文件句柄，`readdir()`获取目录文件下除了`.`与`..`之外的文件名，追加到`fullpath`
    + 访问`fullpath`文件，如果是目录文件，返回前一个步骤
    + 访问`fullpath`文件，如果非目录文件，`lstat`获取文件信息，修改`fullpath`最后一个元素，设置为0或者下一个文件名
    + 当某一个目录文件下所有文件都遍历过，该目录文件的文件描述符`fd`才会被`close()`回收。如果目录文件的层级过深，该进程就会同时打开过多的目录文件的文件描述符`fd`。
- 综上所述，系统如果有对可打开文件数的限制，`myftw`可以同时打开的目录文件的文件描述符数量就会收到限制，访问过深的目录可能导致发生错误。

### 11. 在4.22节中的myftw从不改变其目录，对这种处理方法进行改动，每次遇到一个目录就用其调用chdir，这样每次调用lstat时就可以使用文件名而非路径名，处理完所有的目录项后执行chdir("..")。比较这种版本的程序和书中程序的运行时间。

- 将`myftw`改写后的代码见附录，两份代码都计算实行时间
- 运行结果如下

```
# 原来的ftw
time ../4-22-ftw/main.out ~
regular files  =   49792, 86.06 %
directories    =    7751, 13.40 %
block special  =       0,  0.00 %
char special   =       0,  0.00 %
FIFOs          =       1,  0.00 %
symbolic links =     308,  0.53 %
sockets        =       2,  0.00 %
time: 103773 us
../4-22-ftw/main.out ~  0.02s user 0.08s system 92% cpu 0.105 total

# 改用chdir后的ftw
time ../4-22-ftw-chdir/main.out ~
regular files =   49793, 86.07 %
directories =    7751, 13.40 %
block special =       0,  0.00 %
char special =       0,  0.00 %
FIFOs =       1,  0.00 %
symbolic links =     308,  0.53 %
sockets =       2,  0.00 %
time: 77628 us
./main.out ~  0.01s user 0.07s system 97% cpu 0.079 total
```

- 运行环境：笔者机器是四核cpu，8G内存
- 如结果所示，修改后的改用`chdir`的`ftw`方法运行速度更快，笔者机器上运行看效率差别不大，但是从社区查看别人的测试结果，说相差了一倍。
- 分析两者代码差异
    + 两者的区别在于，原版的需要维护一个静态变量`fullPath`数组，作为当前访问的文件的绝对路径，由于绝对路径的变化，换一个文件就需要修改一次`fullPath`，该过程使用了`realloc()`重新分配`fullPath`的内存大小，每换一次文件就需要调用一次`realloc()`
    + 修改版，一开始需要给`filePath`分配足够大的内存大小存放文件名，切换目录是使用`chdir`修改当前工作目录，修改文件名`filePath`，再访问文件，每换一次目录文件，调用一次`chdir`
    + 综上所述，原版的系统调用执行比修改版的多


### 12. 每个进程都有一个根目录用于解析绝对路径名，可以通过chroot函数改变根目录。在手册查阅此函数。说明这个函数什么时候有用。

- chroot命令用来在指定的根目录下运行指令。chroot，即 change root directory （更改 root 目录）。在 linux 系统中，系统默认的目录结构都是以/，即是以根 (root) 开始的。而在使用 chroot 之后，系统的目录结构将以指定的位置作为/位置。

- 增加了系统的安全性，限制了用户的权力
    + 在经过 chroot 之后，在新根下将访问不到旧系统的根目录结构和文件，这样就增强了系统的安全性。这个一般是在登录 (login) 前使用 chroot，以此达到用户不能访问一些特定的文件。

- 建立一个与原系统隔离的系统目录结构，方便用户的开发
    + 使用 chroot 后，系统读取的是新根下的目录和文件，这是一个与原系统根下文件不相关的目录结构。在这个新的环境中，可以用来测试软件的静态编译以及一些与系统不相关的独立开发。

- 切换系统的根目录位置，引导 Linux 系统启动以及急救系统等
    + chroot 的作用就是切换系统的根位置，而这个作用最为明显的是在系统初始引导磁盘的处理过程中使用，从初始 RAM 磁盘 (initrd) 切换系统的根位置并执行真正的 init。另外，当系统出现一些问题时，我们也可以使用 chroot 来切换到一个临时的系统。


### 13. 如何只设置两个时间值中的一个来使用utimes函数?

utimes与stat都是跟随符号链接，先使用stat获取文件的不需要修改的时间值，再使用utimes修改时间


### 14. 有些版本的finger(1)命令输出`New mail received...`和`unread since...`，其中...表示相应的日期和时间。程序是如何决定这些日期和时间的？

。。。以后再答

### 15. 用cpio(1)和tar(1)命令检查档案文件的格式。3个可能的时间值中哪几个是为每一个文件保存的？你认为文件复原时，文件的访问时间是什么？为什么？

。。。以后再答

### 16. UNIX系统对目录树的深度有限制吗？编写一个程序循环，在每次循环中，创建目录，并将该目录更改为工作目录。确保叶节点的绝对路径名的长度大于系统的PATH_MAX限制。可以调用`getcwd`得到目录的路径名吗？标准UNIX系统工具是如何处理长路径的？对目录可以使用tar或cpio命令归档吗?

。。。以后再答

### 17. 3.16节描述了/dev/fd特征。如果每个用户都可以访问这些文件，则其访问权限必须为rw-rw-rw-。有些程序创建输出文件时，先删除该文件以确保该文件名不存在，忽略返回码。如果path是/dev/fd/1，会出现什么情况?

```c
unlink(path);
if((fd = creat(path, FILE_MODE))<0)
    err_sys(...);
```

。。。以后再答




## 附录

### ftw程序

#### 书上的原版ftw程序，稍作改动，打印出执行的时间

```c
#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <sys/time.h>

/* function type that is called for each filename */
typedef	int	Myfunc(const char *, const struct stat *, int);

static Myfunc	myfunc;
static int		myftw(char *, Myfunc *);
static int		dopath(Myfunc *);

static long	nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int
main(int argc, char *argv[])
{
    struct timeval start, end;
    int timeuse;
	int		ret;

	if (argc != 2)
		err_quit("usage:  ftw  <starting-pathname>");

    gettimeofday( &start, NULL );
	ret = myftw(argv[1], myfunc);		/* does it all */
    gettimeofday( &end, NULL );

	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	if (ntot == 0)
		ntot = 1;		/* avoid divide by 0; print 0 for all counts */
	printf("regular files  = %7ld, %5.2f %%\n", nreg,
	  nreg*100.0/ntot);
	printf("directories    = %7ld, %5.2f %%\n", ndir,
	  ndir*100.0/ntot);
	printf("block special  = %7ld, %5.2f %%\n", nblk,
	  nblk*100.0/ntot);
	printf("char special   = %7ld, %5.2f %%\n", nchr,
	  nchr*100.0/ntot);
	printf("FIFOs          = %7ld, %5.2f %%\n", nfifo,
	  nfifo*100.0/ntot);
	printf("symbolic links = %7ld, %5.2f %%\n", nslink,
	  nslink*100.0/ntot);
	printf("sockets        = %7ld, %5.2f %%\n", nsock,
	  nsock*100.0/ntot);


    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec;
    printf("time: %d us\n", timeuse);

	exit(ret);
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define	FTW_F	1		/* file other than directory */
#define	FTW_D	2		/* directory */
#define	FTW_DNR	3		/* directory that can't be read */
#define	FTW_NS	4		/* file that we can't stat */

static char	*fullpath;		/* contains full pathname for every file */
static size_t pathlen;

static int					/* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
	fullpath = path_alloc(&pathlen);	/* malloc PATH_MAX+1 bytes */
										/* ({Prog pathalloc}) */
	if (pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath, pathname);
	return(dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return.  For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int					/* we return whatever func() returns */
dopath(Myfunc* func)
{
	struct stat		statbuf;
	struct dirent	*dirp;
	DIR				*dp;
	int				ret, n;

	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		return(func(fullpath, &statbuf, FTW_NS));
	if (S_ISDIR(statbuf.st_mode) == 0)	/* not a directory */
		return(func(fullpath, &statbuf, FTW_F));

	/*
	 * It's a directory.  First call func() for the directory,
	 * then process each filename in the directory.
	 */
	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return(ret);

	n = strlen(fullpath);
	if (n + NAME_MAX + 2 > pathlen) {	/* expand path buffer */
		pathlen *= 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	fullpath[n++] = '/';
	fullpath[n] = 0;

	if ((dp = opendir(fullpath)) == NULL)	/* can't read directory */
		return(func(fullpath, &statbuf, FTW_DNR));

	while ((dirp = readdir(dp)) != NULL) {
		if (strcmp(dirp->d_name, ".") == 0  ||
		    strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */
		strcpy(&fullpath[n], dirp->d_name);	/* append name after "/" */
		if ((ret = dopath(func)) != 0)		/* recursive */
			break;	/* time to leave */
	}
	fullpath[n-1] = 0;	/* erase everything from slash onward */

	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);
	return(ret);
}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	case FTW_F:
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:	nreg++;		break;
		case S_IFBLK:	nblk++;		break;
		case S_IFCHR:	nchr++;		break;
		case S_IFIFO:	nfifo++;	break;
		case S_IFLNK:	nslink++;	break;
		case S_IFSOCK:	nsock++;	break;
		case S_IFDIR:	/* directories should have type = FTW_D */
			err_dump("for S_IFDIR for %s", pathname);
		}
		break;
	case FTW_D:
		ndir++;
		break;
	case FTW_DNR:
		err_ret("can't read directory %s", pathname);
		break;
	case FTW_NS:
		err_ret("stat error for %s", pathname);
		break;
	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return(0);
}

```

#### 使用`chdir`的ftw程序修改版

```c
#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <sys/time.h>

typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *k, Myfunc *);
static int dopath(Myfunc *);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


int main(int argc, char *argv[])
{
    struct timeval start, end;
    int ret;
    int timeuse;
    if (argc != 2)
        err_quit("usage: ftw <starting-pathname>");
    gettimeofday( &start, NULL );
    ret = myftw(argv[1], myfunc);
    gettimeofday( &end, NULL );

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;

    if (ntot == 0)
        ntot = 1;

    printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);

    printf("directories = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);

    printf("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);

    printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);

    printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);

    printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);

    printf("sockets = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);

    timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec -start.tv_usec;
    printf("time: %d us\n", timeuse);
    exit(ret);
}

#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

static char filePath[10000]; // set it long not more
static size_t pathlen;
static int myftw(char *pathname, Myfunc *func)
{
    strcpy(filePath, pathname);
    return(dopath(func));
}

static int dopath(Myfunc *func)
{
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;
    char *tmp;

    if (lstat(filePath, &statbuf) < 0)
        return(func(filePath, &statbuf, FTW_NS));

    if (S_ISDIR(statbuf.st_mode) == 0)
        return(func(filePath, &statbuf, FTW_F));

    if ((ret = func(filePath, &statbuf, FTW_D)) != 0)
        return(ret);


    if (chdir(filePath) < 0) 
        err_quit("chdir error");

    if ((dp = opendir(".")) == NULL)
        return(func(filePath, &statbuf, FTW_DNR));

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") == 0)
            continue;
        

        strcpy(filePath, dirp->d_name);
        //printf("%s\n",filePath);
        if ((ret = dopath(func)) != 0)
            break;
    }

    if (closedir(dp) < 0)
        err_ret("can't close directory %s", filePath);

    if (chdir("..") < 0) 
        err_quit("chdir to .. error");
    return(ret);
}


               
static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
    switch (type) {
    case FTW_F:
        switch (statptr->st_mode & S_IFMT) {
        case S_IFREG: nreg++; break;
        case S_IFBLK: nblk++; break;
        case S_IFCHR: nchr++; break;
        case S_IFIFO: nfifo++; break;
        case S_IFLNK: nslink++; break;
        case S_IFSOCK: nsock++; break;
        case S_IFDIR:
            err_dump("for S_IFDIR for %s", pathname);
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        err_ret("can't read directory %s", pathname);
    case FTW_NS:
        err_ret("stat error for %s", pathname);
        break;
    default:
        err_dump("unknown type %d for pathname %s", type, pathname);
    }
    return(0);
}

```