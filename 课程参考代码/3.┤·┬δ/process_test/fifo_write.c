#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    int fd;
    // 有名管道创建完成之后  后续是可以重复使用的   不推荐重复使用  推进每次用完之后释放
    char *pipe_path = "/tmp/myfifo";
    if (mkfifo(pipe_path,0664) != 0)
    {
        perror("mkfifo");
        if (errno != 17)
        {
            exit(EXIT_FAILURE);
        }  
    }

    // 对有名管道的特殊文件 创建fd
    fd = open(pipe_path,O_WRONLY);

    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    char buf[100];
    ssize_t read_num;
    // 读取控制台数据写入到管道中
    while ((read_num = read(STDIN_FILENO,buf,100)) > 0)
    {
        write(fd,buf,read_num);
    }
    
    if (read_num < 0)
    {
        perror("read");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("发送数据到管道完成 进程终止\n");
    close(fd);

    // 释放管道
    // 清除对应的特殊文件
    if (unlink(pipe_path) == -1)
    {
        perror("unlink");
    }
    
    
    return 0;
}
