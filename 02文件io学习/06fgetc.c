#include <stdio.h>


int main()
{
    FILE *ioFile = fopen("io.txt", "r");
    /**
     * @brief 从文件中读取下一个字符
     * @returns：成功时返回读取到的字符，失败时返回EOF。
     * int fgetc (FILE *stream)
     */
    char c = fgetc(ioFile);
    while (c!=EOF)
    {
        printf("%c", c);    //utf-8中文占3个字节 utf-8的编译功能使得它能知道到你要输出的是三个连续字节，输出后它会把三个字节组合起来形成汉字
        c = fgetc(ioFile);
    }
    printf("\n");

    fclose(ioFile);
    return 0;
}