#include <stdio.h>
#include <sys/socket.h> // 包含 socket 相关的结构体与常量（如 AF_INET）
#include <netinet/in.h> // 包含 struct sockaddr_in、struct in_addr、端口/地址类型定义
#include <arpa/inet.h>  // 包含 inet_pton/inet_ntop/inet_aton/inet_addr 等函数原型
#include <string.h>     // 包含 memset
#include <unistd.h>     // 包含 close 函数原型
#include <pthread.h>    // 包含 pthread 相关函数原型
#include <stdlib.h>    // 包含 malloc 和 free 函数原型

#define handle_error(cmd,result) \
    if (result<0)    \
    {                \
        perror(cmd); \
        return -1;   \
    }

void *read_from_client(void *arg)
{
    char *read_buf = NULL;
    int clientFd = *(int *)arg;//强制转换为(int *)后解引用，得到真实的client_fd（如accept()返回的文件描述符）。
    read_buf = (char *)malloc(1024);
    ssize_t count = 0;

    while (count=read(clientFd,read_buf,1024))
    {
        if (count<0)
        {
            perror("recv");
        }
        fputs(read_buf, stdout);
    }
    //ctrl+d 关闭连接
    printf("客户端请求关闭连接\n");
    free(read_buf);
    return NULL;
}

void *write_to_client(void *arg)
{
    char *write_buf = NULL;
    int clientFd = *(int *)arg;//强制转换为(int *)后解引用，得到真实的client_fd（如accept()返回的文件描述符）。
    write_buf = (char *)malloc(1024);
    ssize_t count = 0;

    if (!write_buf) //等价于 if (write_buf == NULL)
    {
        perror("malloc");
        return NULL;
    }

    while (fgets(write_buf,1024,stdin)!=NULL)
    {
        //发送数据
        size_t len = strlen(write_buf);
        count = send(clientFd, write_buf, len, 0);
        if (count<0)
        {
            perror("send");
        }
        
    }
    printf("接收到控制台的关闭请求,不再写入,关闭连接\n");
    //可以具体到关闭哪一端
    shutdown(clientFd, SHUT_WR); // 关闭套接字的写端，告诉客户端“我不再发送数据了”，但仍然可以接收。
    free(write_buf);
    return NULL;
}

int main(int argc, char const *argv[])
{
    int sockFd, clientFd, tempResult;
    struct sockaddr_in server_addr, client_addr; //sockaddr_in是专门为IPv4设计的套接字地址结构体。不带in的sockaddr是通用的套接字地址结构体。
    /* 把结构体清零，养成良好习惯，避免未初始化的数据 */
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    //填写服务端地址信息
    server_addr.sin_family = AF_INET;
    //填写ip地址0.0.0.0
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//s_addr是in_addr_t类型
    //inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr); //另一种写法
    //填写端口号
    server_addr.sin_port = htons(6666);

    //网络编程三部曲之第一步：创建套接字
    sockFd = socket(AF_INET, SOCK_STREAM, 0);//传 0 表示“使用默认协议”。
    handle_error("socket", sockFd);

    //网络编程三部曲之第二步：绑定端口号和ip地址
    //服务器必须通过 bind() 告诉操作系统：“我这个服务要监听 哪个 IP 的哪个端口”。
    tempResult = bind(sockFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    handle_error("bind", tempResult);

    //网络编程三部曲之第三步：监听端口号，等待客户端连接
    //将套接字从 主动套接字（用于发起连接）转变为 被动套接字（用于接收连接），并设置连接请求队列的最大长度。
    tempResult = listen(sockFd, 128);
    handle_error("listen", tempResult);

    //等待客户端连接
    //在调用 accept(fd, (struct sockaddr*)&client_addr, &client_addr_len) 前，client_addr_len 必须包含缓冲区的大小；
    //accept 返回后，client_addr_len 会被修改为实际写入的地址长度。
    socklen_t client_addr_len = sizeof(client_addr);
    //功能：从已完成连接队列中取出一个客户端连接，并返回一个新的套接字描述符，专门用于与该客户端通信。
    //accept() 是阻塞函数（默认情况下），会一直等待直到有客户端连接。
    /**
     * @brief 从已完成连接队列中取出一个客户端连接
     * int accept (int __fd, __SOCKADDR_ARG __addr,socklen_t *__restrict __addr_len);
     * @param __fd 监听套接字的文件描述符
     * @param __addr 指向 struct sockaddr 结构体的指针，用于存储客户端的地址信息
     * @param __addr_len 指向 socklen_t 变量的指针，初始值为 __addr 结构体的大小，返回时更新为实际地址长度
     * @return 成功时返回新创建的套接字描述符，失败时返回 -1 并设置 errno
     */
    clientFd = accept(sockFd, (struct sockaddr *)&client_addr, &client_addr_len);
    handle_error("accept", clientFd);

    printf("与客户端%s %d建立连接，文件描述符:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), clientFd);

    pthread_t pid_read, pid_write;
    //创建子线程用于收消息
    pthread_create(&pid_read, NULL, read_from_client, (void *)&clientFd);
    //创建子线程用于发消息
    pthread_create(&pid_write, NULL, write_to_client, (void *)&clientFd);

    //阻塞主线程
    pthread_join(pid_read, NULL);
    pthread_join(pid_write, NULL);

    printf("关闭连接\n");
    close(clientFd);
    close(sockFd);

    return 0;
}
