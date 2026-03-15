#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    //调用fork之前，代码都在父进程中执行
    printf("fork之前的代码，在父进程中执行，pid=%d\n", getpid());

    //使用fork创建子进程
    /**
     * @brief fork函数用于创建一个新的进程，称为子进程。它会复制调用它的进程（父进程）的地址空间、堆栈和数据段。
     * 不需要传参
     * @return pid_t
     * 返回值有三种情况：
     * 1. 在父进程中，fork返回新创建子进程的进程ID（PID）。
     * 2. 在子进程中，fork返回0。
     * 3. 如果创建进程失败，fork返回-1，并设置errno以指示错误原因。
     * __pid_t fork (void)
     */
    pid_t pid = fork();

    //从fork之后，代码在父进程和子进程中都会执行
    if (pid<0)
    {
        printf("创建子进程失败\n");
        return 1;
    }
    else if(pid==0)
    {
        //子进程执行的代码
        printf("子进程执行的代码，pid=%d, 父进程的pid=%d\n", getpid(), getppid());
    }
    else
    {
        //父进程执行的代码
        printf("父进程执行的代码，pid=%d, 创建的子进程的pid=%d\n", getpid(), pid);
    }
    //fork创建新进程后，所有代码都在父子进程中各执行一次，父进程返回子进程的进程号，子进程返回0

    return 0;
}