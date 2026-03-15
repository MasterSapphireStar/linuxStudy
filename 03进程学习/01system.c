#include <stdio.h>
#include <stdlib.h>

int main()
{
    //使用标准库函数创建子进程
    /**
     * @brief 创建子进程并执行命令
     * @param command 要执行的命令字符串
     * @return 成功返回命令的退出状态，失败返回-1 成功返回0
     * @note system函数用于在子进程中执行一个命令,它会调用操作系统的命令解释器来执行指定的命令
     * int system (const char *__command);
     */
    int sys_result = system("ping -c 10 www.atguigu.com");//-c 表示count，指定发送的包的数量
    if (sys_result!=0)
    {
        perror("system");// 在system后面会拼接上报错信息
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
