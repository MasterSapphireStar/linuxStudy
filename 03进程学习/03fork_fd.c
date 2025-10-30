#include <stdio.h>
#include <unistd.h>   // 包含fork和getpid等函数的声明
#include <sys/stat.h> // 这个头文件包含open函数的声明
#include <fcntl.h>    // 这个头文件包含文件访问模式的定义
#include <stdlib.h>   // 包含exit函数的声明
#include <string.h>   // 包含字符串操作函数的声明


int main()
{
    //fork创建子进程之前，父进程打开一个文件描述符
    int fd = open("io.txt", O_WRONLY | O_CREAT | O_APPEND, 0664);

    if (fd==-1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    char buffer[1024];

    //执行一次open 值由0变为1  fork之后父子进程各自拥有一份文件描述符表的拷贝，文件描述符fd在父子进程中均有效，值均为1 等同于open 值变为2
    pid_t pid = fork();
    if (pid<0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        //子进程执行的代码

        /**
         * @brief 子进程继承了父进程打开的文件描述符fd，因此子进程可以使用该文件描述符进行文件操作。
         * char *strcpy (char *__restrict __dest, const char *__restrict __src)；
         * 功能：把 src 指向的以 '\0' 结尾的字符串复制到 dest 指向的内存中，复制包括终止的空字符 '\0'，复制完成后 dest 中也会是个以 '\0' 结尾的字符串。
         * @param __dest 指向目标字符串的指针
         * @param __src 指向源字符串的指针
         * @return 返回指向目标字符串 dest 的指针
         */
        strcpy(buffer, "子进程写入的数据\n");
    }
    else
    {
        //父进程执行的代码
        sleep(1); // 确保子进程先写入文件
        strcpy(buffer, "父进程写入的数据\n");
    }

    //父子进程都要执行的代码：写入文件
    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written==-1)
    {
        perror("write");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("写入数据成功\n");

    //使用完毕之后关闭文件描述符
    close(fd);

    if (pid == 0)
    {
        printf("子进程结束,并释放文件描述符,pid=%d\n", getpid());
    }
    else
    {
        printf("父进程结束,并释放文件描述符,pid=%d\n", getpid());
    }
    return 0;
}
