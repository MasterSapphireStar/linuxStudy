#include "music_manager.h"

/**
 * @brief 判断是否是音频文件
 * @param filename 
 * @return int 0-不是音频文件 1-是音频文件
 */
int is_audio_file(const char *filename)
{
    /**
     * @brief 判断文件扩展名是否为常见音频格式
     * strrchr (const char *__s, int __c) - 在字符串 __s 中查找最后一次出现字符 __c 的位置
     * @param filename 文件名字符串
     * @return const char* 指向最后一个'.'字符的指针，若未找到则返回NULL
     */
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0; //没有扩展名 返回0

    /**
     * @brief 比较扩展名是否匹配常见音频格式
     * int strcmp (const char *__s1, const char *__s2) - 比较两个字符串的内容是否相同
     * @param ext 文件扩展名
     * @return int 若扩展名匹配返回非0值，否则返回0
     */
    return (strcmp(ext, ".mp3") == 0 ||
        strcmp(ext, ".wav") == 0 ||
        strcmp(ext, ".flac") == 0 ||
        strcmp(ext, ".ogg") == 0 ||
        strcmp(ext, ".m4a") == 0);
}

/**
 * @brief 从文件中加载播放列表 例如 load_playlist(&playlist, "playlist.txt");
 * @param pl 
 * @param file 
 * @return int 加载的歌曲数量，失败返回-1
 */
int load_playlist(playlist_t *pl, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        printf("Warning: Cannot open playlist file '%s'\n", file);
        return -1;
    }
    // 初始化歌曲总数和当前播放索引
    pl->count = 0;
    pl->current_index = 0;

    while (fgets(pl->songs[pl->count].path, MAX_PATH_LEN, fp))//fgets 会将行尾的 换行符 \n 也读入到缓冲区中
    {
        /**
         * @brief 寻找字符串 pl->songs[pl->count].path 中第一个出现的换行符 \n 的位置，并将其替换为字符串结束符 \0，从而去除换行符
         * size_t strcspn(const char *str1, const char *str2)
         * 功能： 计算字符串 str1 开头连续不包含 str2 中任何字符的字符数。简单来说，它找到 str1 中第一个出现在 str2 中的字符的位置（索引）。
         * @param pl->songs[pl->count].path 要处理的字符串
         * @param "\n" 要查找的字符集合，这里是换行符
         * @return size_t 返回 str1 中第一个出现在 str2 中的字符的位置索引
         * strcspn(..., "\n") 找到了 \n 所在的位置索引 $N$  将该位置的字符设置为 0（即空字符 \0），从而去除换行符
         */
        pl->songs[pl->count].path[strcspn(pl->songs[pl->count].path, "\n")] = 0;

        //跳过空行和注释
        if (pl->songs[pl->count].path[0] == '\0' || //检查去除换行符后字符串的第一个字符是否为 \0。如果为 \0，说明这一行是空行
            pl->songs[pl->count].path[0] == '#')
        {
            continue;
        }

        /**
         * @brief 提取文件名部分
         * char *strrchr(const char *str, int c);
         * 功能：在字符串 str 中查找最后一次出现字符 c 的位置，并返回指向该位置的指针。如果未找到该字符，则返回 NULL。
         * @param pl->songs[pl->count].path 要处理的字符串
         * @param '/' 要查找的字符，这里是路径分隔符 '/'
         * @return char* 返回指向最后一个 '/' 字符的指针，若未找到则返回 NULL
         * 例如：路径是"/home/music/song.flac"，filename指向music/song.flac中的最后一个 /。filename+1指向 s，结果name是 "song.flac"。
         */
        char *filename = strrchr(pl->songs[pl->count].path, '/');
        if (filename)
        {
            // 复制跳过路径部分的文件名到歌曲名称字段。
            strcpy(pl->songs[pl->count].name, filename + 1);
        }
        else
        {
            // 如果路径中没有 '/'，则整个路径即为文件名。
            strcpy(pl->songs[pl->count].name, pl->songs[pl->count].path);
        }

        pl->count++;
        if (pl->count >= MAX_SONGS) break;
    }
    fclose(fp);
    printf("✓ Loaded %d songs from playlist\n", pl->count);
    return pl->count;
}
/**
 * @brief 扫描目录加载音乐文件
 * @param pl 
 * @param dir_path 
 * @return int 歌曲数量，失败返回-1
 */
int scan_music_dir(playlist_t *pl, const char *dir_path)
{
    /**
     * @brief 打开目录
     * opendir(const char *name) - 函数用于打开由 dir_path 指定的目录
     * @param dir_path 目录路径字符串
     * @return DIR* 指向打开的目录流的指针，若打开失败则返回 NULL
     */
    DIR *dir = opendir(dir_path);
    if (!dir)
    {
        printf("Warning: Cannot open music directory '%s'\n", dir_path);
        return -1;
    }
    // 初始化歌曲总数和当前播放索引
    pl->count = 0;
    pl->current_index = 0;
    // dirent 结构体用于表示目录中的一个条目（文件或子目录）
    struct dirent *entry; // entry 是一个指向 dirent 结构体的指针，用来存储每次读取的目录项信息
    while ((entry = readdir(dir)) != NULL)
    {
        if (is_audio_file(entry->d_name)) // entry->d_name 是当前目录项的名称（文件名或子目录名）
        {
            /**
             * @brief 构造完整文件路径
             * int snprintf(char *str, size_t size, const char *format, ...);
             * 功能：将格式化的数据写入字符串 str 中，最多写入 size - 1 个字符，并在末尾添加字符串结束符 \0。
             * @param pl->songs[pl->count].path 目标字符串，用于存储构造的完整路径
             * @param MAX_PATH_LEN 目标字符串的最大长度，防止缓冲区溢出
             * @param "%s/%s" 格式字符串，表示路径和文件名之间用 '/' 分隔
             * @param dir_path 目录路径字符串
             * @param entry->d_name 当前目录项的名称（文件名）
             * @return int 返回写入目标字符串的字符数（不包括字符串结束符 \0）
             例如：dir_path 是 "/home/music"，entry->d_name 是 "song.mp3"，则结果路径是 "/home/music/song.mp3"
             */
            snprintf(pl->songs[pl->count].path, MAX_PATH_LEN,
                "%s/%s", dir_path, entry->d_name);
            //把文件名复制到歌曲名称字段
            strcpy(pl->songs[pl->count].name, entry->d_name);
            pl->count++;

            if (pl->count >= MAX_SONGS) break;
        }
    }

    closedir(dir);//关闭目录流
    printf("✓ Scanned %d songs from directory\n", pl->count);
    return pl->count;
}

/**
 * @brief 获取当前歌曲路径
 * @param pl 
 * @return char* 
 */
char *get_current_song(playlist_t *pl)
{
    if (pl->count == 0)
        return NULL;
    return pl->songs[pl->current_index].path;
}
/**
 * @brief 下一首
 * @param pl 
 * @return char*  
 */
char *next_song(playlist_t *pl)
{
    if (pl->count == 0)
        return NULL;
    pl->current_index = (pl->current_index + 1) % pl->count;
    return pl->current_index;
}
/**
 * @brief 上一首
 * @param pl 
 * @return char* 
 */
char *prev_song(playlist_t *pl)
{
    if (pl->count == 0)
        return NULL;
    pl->current_index = (pl->current_index - 1 + pl->count) % pl->count;
    return pl->current_index;
}

// 打印播放列表
void print_playlist(playlist_t *pl)
{
    printf("\n=== Playlist (%d songs) ===\n", pl->count);
    for (int i = 0; i < pl->count; i++)
    {
        printf("[%2d] %s %s\n",
            i,
            (i == pl->current_index) ? "→" : " ",
            pl->songs[i].name);
    }
    printf("========================\n\n");
}
