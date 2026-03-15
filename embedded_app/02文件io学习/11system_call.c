#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
    //打开文件
    int fd = open("io.txt", O_RDONLY);
    if (fd==-1)
    {
        printf("文件打开失败！\n");
        exit(EXIT_FAILURE);// linux系统中，0表示程序正常退出，非0表示异常退出
    }

    char buffer[1024];
    int bytesRead;
    /**
     * @brief 读取文件内容并输出到标准输出
     * __wur ssize_t read (int __fd, void *__buf, size_t __nbytes)
     * @param __fd 文件描述符
     * @param __buf 存储读取数据的缓冲区
     * @param __nbytes 要读取的字节数
     * @return 成功返回读取的字节数，失败返回-1 成功大于0 ssize_t类型->long int
     */
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        //文件描述符类型的 stdin -> 0  stdout -> 1  stderr -> 2
        /**
         * @brief 将数据写入标准输出
         * @param __fd 文件描述符
         * @param __buf 存储要写入数据的缓冲区
         * @param __nbytes 要写入的字节数
         * @return 成功返回写入的字节数，失败返回-1 成功大于0 ssize_t类型->long int
         * ssize_t write (int __fd, const void *__buf, size_t __n)
         */
        write(STDOUT_FILENO, buffer, bytesRead);
    }
    if (bytesRead==-1)
    {
        perror("读取文件失败！");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    return 0;
}