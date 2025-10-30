#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>


int main(int argc, char const *argv[])
{
    char * share;
    // 1 创建一个共享内存对象
    char shm_name[100] = {0};
    sprintf(shm_name,"/letter%d",getpid());
    int fd;
    fd = shm_open(shm_name,O_RDWR | O_CREAT,0644);
    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // 2 设置共享内存对象大小
    ftruncate(fd,1024);
    
    // 3 内存映射
    share = mmap(NULL,1024,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (share == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    // 映射完成之后  关闭fd连接  不是释放
    close(fd);

    // 4 使用内存映射实现进程间的通讯
    pid_t pid = fork();
    if (pid <0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        // 子进程
        strcpy(share,"你是个好人\n");
        printf("新学员%d完成回信\n",getpid());
    }else {
        // 父进程
        waitpid(pid,NULL,0);
        printf("老学员%d收到了新学员%d的回信:%s",getpid(),pid,share);
        // 5 释放映射区
        int re = munmap(share,1024);
        if (re == -1)
        {
            perror("munmap");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // 6 释放共享内存对象
    shm_unlink(shm_name);

    return 0;
}
