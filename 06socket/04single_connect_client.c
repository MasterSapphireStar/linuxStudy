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

void *read_from_server(void *arg)
{
    char *read_buf = NULL;
    int clientFd = *(int *)arg;//强制转换为(int *)后解引用，得到真实的client_fd（如accept()返回的文件描述符）。
    read_buf = (char *)malloc(1024);
    ssize_t count = 0;

    while (count = read(clientFd, read_buf, 1024))
    {
        if (count < 0)
        {
            perror("recv");
        }
        fputs(read_buf, stdout);
    }
    //ctrl+d 关闭连接
    printf("服务端请求关闭连接\n");
    free(read_buf);
    return NULL;
}

void *write_to_server(void *arg)
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

    while (fgets(write_buf, 1024, stdin) != NULL)
    {
        //发送数据
        size_t len = strlen(write_buf);
        count = send(clientFd, write_buf, len, 0);
        if (count < 0)
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
    int clientFd, tempResult;
    struct sockaddr_in server_addr, client_addr;
    /* 把结构体清零，养成良好习惯，避免未初始化的数据 */
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    //填写客户端地址信息
    client_addr.sin_family = AF_INET;
    //填写ip地址0.0.0.0
    inet_pton(AF_INET, "192.168.0.101", &client_addr.sin_addr); //第三个参数dst 指向一个 struct in_addr（当 af == AF_INET 时）。
    //填写端口号
    client_addr.sin_port = htons(8888);

    // 填写服务端地址
    server_addr.sin_family = AF_INET;
    // 填写ip地址  
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    // 填写端口号
    server_addr.sin_port = htons(6666);

    //客户端网络编程流程
    //1.创建socket套接字
    clientFd = socket(AF_INET, SOCK_STREAM, 0);//传 0 表示“使用默认协议”。
    handle_error("socket", clientFd);

    //2.绑定
    //如果不需要指定客户端的ip和端口，可以省略这一步，系统会自动分配一个临时的ip和端口。
    tempResult = bind(clientFd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    handle_error("bind", tempResult);

    //3.连接服务器
    tempResult = connect(clientFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    handle_error("connect", tempResult);


    printf("连接上服务端%s %d，文件描述符:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), clientFd);

    pthread_t pid_read, pid_write;
    //创建子线程用于收消息
    pthread_create(&pid_read, NULL, read_from_server, (void *)&clientFd);
    //创建子线程用于发消息
    pthread_create(&pid_write, NULL, write_to_server, (void *)&clientFd);

    //阻塞主线程
    pthread_join(pid_read, NULL);
    pthread_join(pid_write, NULL);

    printf("关闭连接\n");
    close(clientFd);

    return 0;
}
