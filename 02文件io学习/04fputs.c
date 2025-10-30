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
     * @brief 向文件写入一个字符，读写权限记录在fopen时的模式中
     * int __c：ASCII码对应的字符。
     * FILE *stream：文件指针，指向要写入的文件。
     * @returns：成功时返回写入的字符，失败时返回EOF。
     * int fputc (int __c, FILE *__stream)
     */
    int putc_result = fputc(97, ioFile); //向文件写入字符'a'，ASCII码为97
    if (putc_result == EOF) {
        printf("failed 写入字符失败\n");
    }
    else {
        printf("succeed 写入字符成功\n");
    }

    /**
     * @brief 向文件写入一个字符串，读写权限记录在fopen时的模式中
     * const char *__s：要写入的字符串。
     * FILE *stream：文件指针，指向要写入的文件。
     * @returns：成功时返回非负值，失败时返回EOF。
     * int fputs (const char *__restrict __s, FILE *__restrict __stream)
     */
    int puts_result = fputs(" love letter\n", ioFile); //向文件写入字符串
    if (puts_result == EOF)
        printf("failed 写入字符串失败\n");
    else
        printf("succeed 写入字符串%d成功\n", puts_result);

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
