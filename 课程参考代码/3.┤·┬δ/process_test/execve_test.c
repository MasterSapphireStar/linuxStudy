#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    // 跳转之前
    char *name = "banzhang";
    printf("我是%s 编号%d 父进程%d,我现在还在一楼\n",name,getpid(),getppid());

    // 执行跳转
    /**
     * const char *__path: 执行程序的路径
     * char *const __argv[]: 传入的参数 -> 对应执行程序main方法的第二个参数
     *      (1): 第一个参数固定是程序的名称 -> 执行程序的路径
     *      (2): 执行程序需要传入的参数
     *      (3): 最后一个参数一定是NULL
     * char *const __envp[]: 传递的环境变量
     *      (1): 环境变量参数: key=value
     *      (2): 最后一个参数一定是NULL
     * return: 成功根本没办法返回  下面的代码也没有意义
     *          失败返回-1
     * 跳转前后只有进程号保留下来  别的变量都删除了
     * int execve (const char *__path, char *const __argv[], char *const __envp[])
    */
    char *args[] = {"/home/atguigu/process_test/erlou",name,NULL};
    char *envs[] = {"PATH=/home/atguigu/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin",NULL};
    int re = execve(args[0],args,envs);
    if (re == -1)
    {
        printf("你没有机会上二楼\n");
        return 1;
    }
    
    // 此处的代码没有意义  因为程序跳转了  不会再往下执行了

    return 0;
}
