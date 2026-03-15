#include <stdio.h>
#include <sys/socket.h> // 包含 socket 相关的结构体与常量（如 AF_INET）
#include <netinet/in.h> // 包含 struct sockaddr_in、struct in_addr、端口/地址类型定义
#include <arpa/inet.h>  // 包含 inet_pton/inet_ntop/inet_aton/inet_addr 等函数原型
#include <string.h>     // 包含 memset

/*
 * 该文件演示了多种将点分十进制 IPv4 地址转换为二进制表示的函数。
 * 注：我只添加了注释，未改变任何程序逻辑或行为。
 */
int main(int argc, char const *argv[])
{
    /* 直观地把 IP 的四个字节以十六进制打印出来，便于观察 */
    printf("192.168.6.101的16进制表示为0x%02X %02X %02X %02X\n", 192, 168, 6, 101);

    /*
     * 结构体说明（简要）：
     * - struct in_addr: 包含 in_addr_t s_addr，用来存放 IPv4 地址（二进制，网络字节序）
     * - struct sockaddr_in: IPv4 地址+端口的套接字结构体，包含 sin_family、sin_port、sin_addr 等
     */
     // 声明用于演示的变量
    struct sockaddr_in server_addr; // 用于保存拼接后的 sockaddr_in（包含 sin_addr）
    struct in_addr server_in_addr;  // 单个 IPv4 地址结构（s_addr 存为网络字节序）
    in_addr_t server_in_addr_t;     // 旧接口 inet_addr 返回的整型类型

    /* 把结构体清零，养成良好习惯，避免未初始化的数据 */
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&server_in_addr, 0, sizeof(server_in_addr));
    memset(&server_in_addr_t, 0, sizeof(server_in_addr_t));

    /*
     * inet_addr
     * - 把点分十进制字符串转换为 in_addr_t（32位）并返回
     * - 出错或无效时返回 INADDR_NONE（0xFFFFFFFF），但这个值同时也是合法地址 255.255.255.255，
     *   因此不能用返回值单独可靠判断错误，故不推荐在新代码中使用
     */
    server_in_addr_t = inet_addr("192.168.6.101");
    printf("inet_addr函数转换后的结果为0x%X\n", server_in_addr_t);

    /*
     * inet_aton
     * - 把点分十进制字符串转换为 struct in_addr（结果写入第二个参数）
     * - 返回非0表示成功，返回0表示字符串无效
     * - 相比 inet_addr，inet_aton 可以通过返回值可靠判断错误
     */
    inet_aton("192.168.6.101", &server_in_addr);
    printf("inet_aton函数转换后的结果为0x%X\n", server_in_addr.s_addr);

    /*
     * inet_pton
     * - 推荐的通用函数，支持 IPv4 和 IPv6（AF_INET/AF_INET6）
     * - 返回 1 成功，0 字符串不是有效的地址，-1 address family 不支持
     * - 将结果以网络字节序写入目标（这里写入 server_in_addr.s_addr）
     */
    inet_pton(AF_INET, "192.168.6.101", &server_in_addr.s_addr);
    printf("inet_pton函数转换后的结果为0x%X\n", server_in_addr.s_addr);


    /*
     * inet_ntoa
     * - 将 struct in_addr（网络字节序）转换为点分十进制字符串
     * - 返回指向静态缓冲区的指针（非线程安全），在多线程或需要持久化字符串时请使用 inet_ntop
     */
    printf("inet_ntoa函数转换后的结果为%s\n", inet_ntoa(server_in_addr));

    /*
     * 以下为历史遗留的类网络（classful）辅助函数：
     * - inet_lnaof: 返回主机号（host part，局部主机地址）
     * - inet_netof: 返回网络号（network part）
     * - inet_makeaddr: 根据网络号和主机号构造一个 struct in_addr
     *
     * 这些函数基于早期的 A/B/C 类划分，现代网络中通常使用掩码（CIDR），
     * 因此不推荐作为常规方法来拆分网络/主机部分，仅作演示/兼容用。
     */
    printf("本地网络地址转换后的结果为%X\n", inet_lnaof(server_in_addr));
    printf("网络号地址转换后的结果为%X\n", inet_netof(server_in_addr));

    /* 使用 inet_makeaddr 把网络号和主机号拼回地址（演示用途） */
    server_addr.sin_addr = inet_makeaddr(inet_netof(server_in_addr), inet_lnaof(server_in_addr));
    printf("拼接后的地址结果为:%s\n", inet_ntoa(server_addr.sin_addr));

    return 0;
}
