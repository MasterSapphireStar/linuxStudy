#include <stdio.h>


int main()
{
    FILE *ioFile = fopen("io.txt", "r");
    if (!ioFile) {
        perror("fopen");
        return 1;
    }

    /**
     * @brief 从文件中读取一行字符串
     * @returns：成功时返回指向字符串的指针，失败时返回NULL。
     * char *__restrict __s：指向用于存储读取字符串的缓冲区的指针，__restrict表示用户确定该指针是唯一指向该对象的指针。
     * int __n：要读取的最大字符数，包括结尾的空字符。
     * FILE *__restrict __stream：文件指针，指向要读取的文件，__restrict表示用户确定该指针是唯一指向该对象的指针。
     * char *fgets (char *__restrict __s, int __n,
            FILE *__restrict __stream)
     */

    char buffer[100];
    while (fgets(buffer,sizeof(buffer),ioFile))
    {
        printf("%s", buffer);
    }
    printf("\n");

    fclose(ioFile);
    return 0;
}