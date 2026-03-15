#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[])
{
    //声明两个端口号
    unsigned short local_port = 0x1f, network_port = 0;

    network_port = htons(local_port); // 将主机字节序转换为网络字节序
    // x86架构是小端字节序，即0x1f在低地址，0x00在高地址，转换为网络字节序（大端字节序）后，高地址存放0x1f，低地址存放0x00
    printf("将主机字节符无符号整数0x%04hx转换为网络字节符0x%04hx\n", local_port, network_port);

    local_port = ntohs(network_port); // 将网络字节序转换为主机字节序
    // 将网络字节序（大端字节序）转换为主机字节序（小端字节序）后，低地址存放0x1f，高地址存放0x00
    printf("将网络字节符无符号整数0x%04hx转换为主机字节符0x%04hx\n", network_port, local_port);

   return 0;
}

