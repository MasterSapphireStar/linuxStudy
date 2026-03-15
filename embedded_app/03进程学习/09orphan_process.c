#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for wait

int main(int argc, char const *argv[])
{
    //邀请之前 
    char *name = "老学员";
    printf("%s,编号%d,继续在一楼精进学习\n", name, getpid());

    //邀请新学员
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("邀请新学员失败了\n");
        return 1;
    }
    else if (pid == 0)
    {
        //子进程  新学员
        char *new_name = "新学员";
        char *args[] = { "/home/msi/Codes/03进程学习/09erlou_block", new_name, NULL };
        char *envs[] = { NULL };
        //如果 execve 成功，则后面的代码确实不会被执行，也不会返回值
        int exec_result = execve(args[0], args, envs);
        if (exec_result == -1)
        {
            printf("新学员上二楼失败了，因为execve跳转失败了\n");
            return 1;
        }
        //此处代码没有意义 因为程序跳转了 不会再往下执行了
    }
    else
    {
        //父进程  老学员
        //等待新学员上完二楼再继续精进学习
        //wait(NULL); //等待子进程结束 否则父进程id号会发生变化
        //sleep(1);  //和wait效果类似 只是单纯等待时间
        printf("%s,编号%d,新学员上完二楼了，我继续在一楼精进学习\n", name, getpid());
        //char byte = fgetc(stdin); //阻塞等待用户输入一个字符 永久挂起
    }
    return 0;
}
