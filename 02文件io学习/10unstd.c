#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    /**
     * int open (const char *__path, int __oflag, ...)
     * @brief 未标准化函数示例
     * @param __path 文件路径
     * @param __oflag 文件打开标志
     * (1) O_RDONLY 以只读方式打开文件
     * (2) O_WRONLY 以只写方式打开文件
     * (3) O_RDWR 以读写方式打开文件
     * (4) O_CREAT 如果文件不存在则创建文件  
     * (5) O_TRUNC 如果文件已存在且以写入方式打开，则将其长度截断为0
     * (6) O_APPEND 以追加方式写入文件
     * @param ... 可选的文件权限参数  创建文件的权限：八进制数 0664：rw-rw-r--
     * @return 成功返回文件描述符，失败返回-1
     */
    int fd = open("io1.txt", O_RDONLY | O_CREAT, 0664);//linux有文件保护机制，默认创建的文件会删除其他用户的写权限  默认权限是0644
    if (fd==-1)
    {
        printf("文件打开失败！\n");
        return -1;
    }   
    close(fd);
    return 0;
}