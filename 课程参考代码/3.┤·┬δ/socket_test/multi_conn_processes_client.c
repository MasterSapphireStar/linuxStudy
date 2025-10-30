#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define handle_error(cmd, result) \
    if (result < 0)               \
    {                             \
        perror(cmd);              \
        return -1;                \
    }

void *read_from_server(void * arg){
    // 使用recv接收客户端发送的数据  打印到控制台
    char * read_buf = NULL;
    int client_fd = *(int *)arg;
    read_buf = malloc(sizeof(char) * 1024);
    ssize_t count = 0;

    if (!read_buf)
    {
        perror("malloc server read_buf");
        return NULL;
    }

    // 接收数据
    // 只要能接收到数据 正常使用 一直挂起
    while (count = recv(client_fd,read_buf,1024,0))
    {
        if (count < 0)
        {
            perror("recv");
        }
        
        fputs(read_buf,stdout);
    }
    
    printf("服务端请求关闭\n");
    free(read_buf);

    return NULL;

}

void *write_to_server(void * arg){
    // 接收控制台输入的信息  写出去
    char * write_buf = NULL;
    int client_fd = *(int *)arg;
    write_buf = malloc(sizeof(char) * 1024);
    ssize_t count = 0;

    if (!write_buf)
    {
        perror("malloc server read_buf");
        return NULL;
    }

    while (fgets(write_buf,1024,stdin) != NULL)
    {
        // 发送数据
        count =  send(client_fd,write_buf,1024,0);
        if (count < 0)
        {
            perror("send");
        }
        
    }

    printf("接收到控制台的关闭请求 不再写入  关闭连接\n");
    // 可以具体到关闭某一端
    shutdown(client_fd,SHUT_WR);
    free(write_buf);
    return NULL;
    

}

int main(int argc, char const *argv[])
{
    int sockfd,temp_result;
    pthread_t pid_read,pid_write;
    struct sockaddr_in server_addr,client_addr;
    // 清空
    memset(&server_addr,0,sizeof(server_addr));
    memset(&client_addr,0,sizeof(client_addr));


    // 填写服务端地址
    server_addr.sin_family = AF_INET;
    // 填写ip地址  
    inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);
    // 填写端口号
    server_addr.sin_port = htons(6666);

    // 客户端网络编程流程
    // 1. 创建socket
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    handle_error("socket",sockfd);

    // 2. 绑定
    // 客户端可以不绑定  系统会自动分配一个空闲的端口号给客户端

    // 3. 主动连接服务端
    temp_result = connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    handle_error("connect",temp_result);

    printf("连接上服务端%s %d\n",inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
    
    // 创建子线程用于收消息
    pthread_create(&pid_read,NULL,read_from_server,(void * )&sockfd);
    // 创建子线程用于发消息
    pthread_create(&pid_write,NULL,write_to_server,(void * )&sockfd);

    // 阻塞主线程
    pthread_join(pid_read,NULL);
    pthread_join(pid_write,NULL);

    printf("释放资源\n");
    close(sockfd);

    return 0;
}
