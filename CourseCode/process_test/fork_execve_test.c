#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    // 邀请之前
    char *name = "老学员";

    printf("%s %d继续在一楼精进\n",name,getpid());

    // 邀请新学员
    __pid_t pid =  fork();
    if (pid == -1)
    {
        printf("邀请新学员失败\n");
        return 1;
    }else if (pid == 0)
    {
        // 新学员在这里
        char *new_name = "ergou";
        char *args[] = {"/home/atguigu/process_test/erlou",new_name,NULL};
        char *envs[] = {NULL};
        int exR = execve(args[0],args,envs);
        if (exR == -1)
        {
            printf("新学员上二楼失败\n");
            return 1 ;
        }
        // 新学员上二楼成功  此处代码不执行
        
    }else {
        // 老学员在这
        sleep(1);
        printf("老学员%d 邀请完%d之后还是在一楼学习\n",getpid(),pid);
    }
    return 0;
}
