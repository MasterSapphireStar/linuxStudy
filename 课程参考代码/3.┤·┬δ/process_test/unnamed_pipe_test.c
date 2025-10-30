#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char const *argv[])
{

    pid_t cpid;
    int pipefd[2];
    // 将程序传递进来的第一个命令行参数  通过管道传输给子进程
    if (argc != 2)
    {
        fprintf(stderr,"%s:请填写需要传递的信息\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 创建管道
    if (pipe(pipefd) == -1)
    {
        perror("创建管道失败");
        exit(EXIT_FAILURE);
    }
    

    // 复制父子进程
    cpid = fork();
    if (cpid == -1)
    {
        perror("邀请新学员失败!");
        exit(EXIT_FAILURE);
    }
    
    if (cpid == 0)
    {
        // 子进程 读取管道的数据  打印到控制台
        close(pipefd[1]);
        char str[100] = {0};
        sprintf(str,"新学员 %d接收信息\n",getpid());
        write(STDOUT_FILENO,str,sizeof(str));
        char buf;
        while (read(pipefd[0],&buf,1) > 0)
        {
            write(STDOUT_FILENO,&buf,1);
        }
        write(STDOUT_FILENO,"\n",1);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    }else {
        // 父进程 写入管道数据   提供给子进程读
        close(pipefd[0]);
        // 将数据写入到管道中
        printf("老学员%d对新学员传递信息\n",getpid());
        write(pipefd[1],argv[1],strlen(argv[1]));
        close(pipefd[1]);
        waitpid(cpid,NULL,0);
        exit(EXIT_SUCCESS);
    }
    

    
    return 0;
}
