#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include <stdio.h>  // For FILE operations FILE*, fopen, fclose, fgets
#include <stdlib.h> // For malloc, free
#include <string.h> // For strcpy, strcmp
#include <dirent.h> // For directory operations, such as opendir, readdir, closedir

#define MAX_SONGS 100 //
#define MAX_PATH_LEN 512

//歌曲信息结构体
typedef struct
{
    char path[MAX_PATH_LEN];  //完整路径
    char name[128];           //歌曲名称
}song_info_t;

//播放列表结构体
typedef struct
{
    song_info_t songs[MAX_SONGS]; //歌曲数组
    int count;           //歌曲总数
    int current_index;   //当前播放索引
}playlist_t;

int is_audio_file(const char *filename);
int load_playlist(playlist_t *pl, const char *file);
int scan_music_dir(playlist_t *pl, const char *dir_path);
char *get_current_song(playlist_t *pl);
char *next_song(playlist_t *pl);
char *prev_song(playlist_t *pl);
void print_playlist(playlist_t *pl);

#endif
