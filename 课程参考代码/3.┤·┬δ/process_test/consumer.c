#include <time.h>
#include <mqueue.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // 创建消息队列
    struct mq_attr attr;
    
    // 有用的参数  表示消息队列的容量
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 100;
    // 被忽略的消息 在创建消息队列的时候用不到
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;
    
    char *mq_name = "/p_c_mq";
    mqd_t mqdes =  mq_open(mq_name,O_RDWR | O_CREAT,0664,&attr);


    if (mqdes == (mqd_t)-1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // 不断接收控制台中的数据  发送到消息队列
    char read_buf[100];
    struct timespec time_info;
    while (1)
    {
        // 清空缓冲区
        memset(read_buf,0,100);
        
        clock_gettime(0,&time_info);
        time_info.tv_sec += 15;
        // 读取消息队列的一条数据
        if (mq_timedreceive(mqdes,read_buf,100,NULL,&time_info) == -1)
        {
            perror("mq_timedreceive");
        }
        // 判断当前数据是否为结束信息
        if (read_buf[0] == EOF)
        {
            printf("接收到生产者发送的结束信息  准备退出...\n");
            break;
        }

        // 正常读取的消息队列信息  打印到控制台
        printf("接收到来自生产者发送的信息%s\n",read_buf);
    }
    
    // 关闭消息队列描述符
    close(mqdes);

    // 清除消息队列
    mq_unlink(mq_name);

    return 0;
}
