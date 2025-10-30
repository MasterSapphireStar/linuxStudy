#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for wait

int main(int argc, char const *argv[])
{
    //fork之前
    int subProcess_status;
    printf("老学员在校区\n");

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    else if (pid==0)
    {
        //新学员
        char *args[]={"/usr/bin/ping","-c","4","www.atguigu.com",NULL};
        char *envs[] = { NULL };
        int exec_error = execve(args[0], args, envs);
        if (exec_error < 0)
        {
            perror("execve");
            return 1;
        }
    }
    else
    {
        //老学员
        printf("老学员%d,等待新学员%d联系\n", getpid(), pid);
        /**
         * @brief waitpid 用于等待指定的子进程结束
         * __pid_t waitpid (__pid_t __pid, int *__stat_loc, int __options);
         * @param __pid 指定要等待的子进程id号  如果为-1表示等待任意子进程
         * @param __stat_loc 用于存储子进程退出状态的地址  如果不关心可以传NULL
         * @param __options 选项  一般传0表示阻塞等待
         * @return 成功返回被等待的子进程id号  失败返回-1
         */
        waitpid(pid, &subProcess_status, 0);
    }
    printf("新学员%d联系完毕，老学员%d继续学习\n", pid, getpid());

    return 0;
}

