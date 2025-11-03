#include "net_server.h"

void *read_from_client(void *arg)
{
    int clientfd = *(int *)arg;// 将void指针转换为int指针并解引用
    free(arg);                // 释放传入的内存
    char buf[256];

    while (1)
    {
        //接收客户端命令
        int n = recv(clientfd, buf, sizeof(buf) - 1, 0);
        if (n<=0)
        {
            //接收出错或客户端关闭连接
            printf("Client disconnected\n");
            break;
        }
        buf[n] = '\0'; //确保字符串以null结尾
        printf("Received command: %s\n", buf);
    }
    close(clientfd);    //关闭客户端连接
    pthread_exit(NULL); //退出线程
}

void net_server_init()
{
    int sockfd, clientfd, tempDebug;
    //1. 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//第三个参数为0表示使用默认协议
    //2. 绑定地址和端口
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY, //监听所有网卡
        .sin_port = htons(6666)        //端口号
    };
    tempDebug = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    handle_error("bind", tempDebug);

    //3. 监听端口
    tempDebug = listen(sockfd, 5); //最大连接数为5
    handle_error("listen", tempDebug);

    //4. 循环接受客户端连接
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        handle_error("accept", clientfd);

        //为每个客户端创建一个线程进行处理
        pthread_t tid;
        int *pclient = malloc(sizeof(int));
        *pclient = clientfd;
        if (pthread_create(&tid,NULL,read_from_client,pclient)!=0)
        {
            perror("pthread_create failed");
            close(clientfd);
            free(pclient);
            continue;//如果线程创建失败，则关闭客户端连接并释放内存，然后继续接受下一个连接
        }
        pthread_detach(tid); //分离线程，避免内存泄漏
    }

    close(sockfd); //关闭服务器套接字
}