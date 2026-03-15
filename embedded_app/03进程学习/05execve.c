#include <stdio.h>
#include <stdlib.h> // for exit
#include <unistd.h>

int main(int argc, char const *argv[])
{
    //跳转之前
    char *name = "banzhang";
    printf("我是%s,编号%d,父进程%d,我现在在一楼\n", name, getpid(), getppid());

    //执行跳转
    char *args[] = { "/home/msi/Codes/03进程学习/04erlou", name, NULL };
    char *envs[] = { "PATH",NULL };
    int ret = execve(args[0], args, envs);
    if (ret==-1)
    {
        printf("你没有机会上二楼，因为execve跳转失败了\n");
        return 1;
    }

    // 此处的代码没有意义  因为程序跳转了  不会再往下执行了

    return 0;
}
