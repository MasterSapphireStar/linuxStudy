#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//“左 const 管内容，右 const 管指针” 
/**
 * @brief 二楼程序
 *
 * @param argc 命令行参数总数（含程序名）
 * @param argv 字符串数组，argv[0]是程序名，argv[1..argc-1]是用户参数
 * @return int
 */
int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("参数不够，不能上二楼\n");
        return 1;
    }
    printf("我是%s,编号%d,父进程%d,我跟海哥上二楼了\n", argv[1], getpid(), getppid());
    sleep(100); //挂起线程
    return 0;
}