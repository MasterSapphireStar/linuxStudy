#include <stdio.h>

int main()
{
    /**
     *
     * char *__restrict __filename：文件名，__restrict表示用户确定该指针是唯一指向该对象的指针
     * char *__restrict __modes：文件打开模式，__restrict表示用户确定该指针是唯一指向该对象的指针
     *     modes参数可以是以下值之一或其组合：
     *     "r"：以只读方式打开文件，如果没有文件就报错。
     *     "w"：以只写方式打开文件，如果文件存在则清空文件内容，如果文件不存在则创建新文件。
     *     "a"：以追加方式打开文件，如果文件存在则将写入操作追加到文件末尾，如果文件不存在则创建新文件。
     *     "r+"：以读写方式打开文件，文件必须存在。
     *     "w+"：以读写方式打开文件，如果文件存在则清空文件内容，如果文件不存在则创建新文件。
     *     "a+"：以读写方式打开文件，如果文件存在则将写入操作追加到文件末尾，如果文件不存在则
     * @returns：成功时返回文件指针，失败时返回NULL。
     * FILE *fopen (const char *__restrict __filename,
            const char *__restrict __modes)
     */
    char *fileName = "io.txt";
    FILE *ioFile = fopen(fileName, "w");
    if (ioFile == NULL) {
        printf("failed 打开文件失败\n");
    }
    else {
        printf("succeed 打开文件成功\n");
        //fclose(ioFile);
    }

    /**
     * @brief 关闭文件
     * FILE *stream：文件指针，指向要关闭的文件。
     * @returns：成功时返回0，失败时返回EOF。
     * int fclose (FILE *stream)
     */
    int result = fclose(ioFile);
    if(result == 0) {
        printf("succeed 关闭文件成功\n");
    }
    else {
        printf("failed 关闭文件失败\n");
    }
    return 0;
}
