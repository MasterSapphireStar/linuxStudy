#include "player.h"
#include "music_manager.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("=== Music Player Test ===\n\n");

    // 1. 初始化播放器
    player_context_t player;
    player_init(&player);

    // 2. 加载播放列表
    playlist_t playlist;
    if (scan_music_dir(&playlist, "./music") <= 0) {
        printf("No music files found in ./music directory\n");
        printf("Creating test playlist...\n");
        // 你可以手动指定测试文件
        strcpy(playlist.songs[0].path, "/path/to/test.mp3");
        strcpy(playlist.songs[0].name, "test.mp3");
        playlist.count = 1;
        playlist.current_index = 0;
    }

    print_playlist(&playlist);

    // 3. 播放第一首歌
    char *song = get_current_song(&playlist);
    if (song) {
        player_start(&player, song);

        // 播放5秒
        sleep(5);

        // 暂停
        player_pause(&player);
        sleep(2);

        // 继续
        player_resume(&player);
        sleep(3);

        // 停止
        player_stop(&player);
    }

    // 4. 清理
    player_cleanup(&player);

    printf("\n=== Test completed ===\n");
    return 0;
}
