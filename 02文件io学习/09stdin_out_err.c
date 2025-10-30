#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *ch = (char *)malloc(20);
    //从标准输入读取字符串
    fgets(ch, 20, stdin);
    printf("你输入的字符串是: %s\n", ch);

    //标准输出
    fputs(ch, stdout);

    //标准错误输出
    fputs(ch, stderr);

    free(ch);
    return 0;
}