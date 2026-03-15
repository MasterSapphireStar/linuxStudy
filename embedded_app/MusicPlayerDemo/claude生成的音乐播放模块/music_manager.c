#include "music_manager.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h> // For directory operations

// 判断是否是音频文件
int is_audio_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;

    return (strcmp(ext, ".mp3") == 0 ||
        strcmp(ext, ".wav") == 0 ||
        strcmp(ext, ".flac") == 0 ||
        strcmp(ext, ".ogg") == 0 ||
        strcmp(ext, ".m4a") == 0);
}

// 从文件加载播放列表
int load_playlist(playlist_t *pl, const char *file) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        printf("Warning: Cannot open playlist file '%s'\n", file);
        return -1;
    }

    pl->count = 0;
    pl->current_index = 0;

    while (fgets(pl->songs[pl->count].path, MAX_PATH_LEN, fp)) {
        // 去除换行符
        pl->songs[pl->count].path[strcspn(pl->songs[pl->count].path, "\n")] = 0;

        // 跳过空行和注释
        if (pl->songs[pl->count].path[0] == '\0' ||
            pl->songs[pl->count].path[0] == '#') {
            continue;
        }

        // 提取文件名
        char *filename = strrchr(pl->songs[pl->count].path, '/');
        if (filename) {
            strcpy(pl->songs[pl->count].name, filename + 1);
        }
        else {
            strcpy(pl->songs[pl->count].name, pl->songs[pl->count].path);
        }

        pl->count++;
        if (pl->count >= MAX_SONGS) break;
    }

    fclose(fp);
    printf("✓ Loaded %d songs from playlist\n", pl->count);
    return pl->count;
}

// 扫描目录加载音乐文件
int scan_music_dir(playlist_t *pl, const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Warning: Cannot open music directory '%s'\n", dir_path);
        return -1;
    }

    pl->count = 0;
    pl->current_index = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (is_audio_file(entry->d_name)) {
            // 构造完整路径
            snprintf(pl->songs[pl->count].path, MAX_PATH_LEN,
                "%s/%s", dir_path, entry->d_name);

            strcpy(pl->songs[pl->count].name, entry->d_name);
            pl->count++;

            if (pl->count >= MAX_SONGS) break;
        }
    }

    closedir(dir);
    printf("✓ Found %d audio files in '%s'\n", pl->count, dir_path);
    return pl->count;
}

// 获取当前歌曲路径
char *get_current_song(playlist_t *pl) {
    if (pl->count == 0) return NULL;
    return pl->songs[pl->current_index].path;
}

// 下一首
char *next_song(playlist_t *pl) {
    if (pl->count == 0) return NULL;
    pl->current_index = (pl->current_index + 1) % pl->count;
    return pl->songs[pl->current_index].path;
}

// 上一首
char *prev_song(playlist_t *pl) {
    if (pl->count == 0) return NULL;
    pl->current_index = (pl->current_index - 1 + pl->count) % pl->count;
    return pl->songs[pl->current_index].path;
}

// 打印播放列表
void print_playlist(playlist_t *pl) {
    printf("\n=== Playlist (%d songs) ===\n", pl->count);
    for (int i = 0; i < pl->count; i++) {
        printf("[%2d] %s %s\n",
            i,
            (i == pl->current_index) ? "→" : " ",
            pl->songs[i].name);
    }
    printf("========================\n\n");
}
