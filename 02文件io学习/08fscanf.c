#include <stdio.h>


int main()
{
    FILE *ioFile = fopen("user.txt", "r");
    if (!ioFile) {
        perror("fopen");
        return 1;
    }

    char name[50];
    int age;
    char game[50];

    /**
     * @brief 从文件中读取格式化输入
     * @returns：成功时返回成功赋值的项数，失败时返回EOF
     * int fscanf (FILE *__restrict __stream,
            const char *__restrict __format, ...)
        FILE *__restrict __stream：文件指针，指向要读取的文件，__restrict表示用户确定该指针是唯一指向该对象的指针。
        const char *__restrict __format：格式字符串，指定要读取的数据的格式，__restrict表示用户确定该指针是唯一指向该对象的指针。（固定格式接收）
        ...：可变参数列表，根据格式字符串提供的格式依次传入相应的参数的地址。（接收数据提前声明的变量）
     */
    int scanf_result;
    //文件流有一个“当前位置”（file position）。每次调用 `fscanf` / `fgets` / `fread` 都会从当前位置读取数据并推进这个位置到下一个未读的位置。  
    while ((scanf_result = fscanf(ioFile, "%s %d %s", name, &age, game)) != EOF)
    {
        printf("成功匹配到的参数有：%d个\n", scanf_result);
        printf("Name: %s, Age: %d, Game: %s\n", name, age, game);
    }

    fclose(ioFile);
    return 0;
}