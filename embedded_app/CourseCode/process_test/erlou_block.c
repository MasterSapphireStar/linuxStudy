#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("参数不够,不能上二楼\n");
        return 1;
    }
    printf("我是%s 编号%d父进程%d,我跟海哥上二楼啦\n",argv[1],getpid(),getppid());
    // 挂起进程
    sleep(100);
    return 0;
}
