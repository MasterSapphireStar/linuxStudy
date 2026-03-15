#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#define MAX_SONGS 100
#define MAX_PATH_LEN 512

// 歌曲信息结构
typedef struct {
    char path[MAX_PATH_LEN];  // 完整路径
    char name[128];           // 歌曲名称
    int duration;             // 时长（秒），可选
} song_info_t;

// 播放列表结构
typedef struct {
    song_info_t songs[MAX_SONGS];
    int count;           // 歌曲总数
    int current_index;   // 当前播放索引
} playlist_t;

// 函数声明
int load_playlist(playlist_t *pl, const char *file);
int scan_music_dir(playlist_t *pl, const char *dir_path);
char *get_current_song(playlist_t *pl);
char *next_song(playlist_t *pl);
char *prev_song(playlist_t *pl);
int is_audio_file(const char *filename);
void print_playlist(playlist_t *pl);

#endif
