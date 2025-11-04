#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <stdio.h>
#include <stdlib.h> // For malloc, free
#include <string.h> // For strcpy, strcmp
#include <pthread.h>// For pthreads
#include <semaphore.h>// For semaphores
#include <unistd.h> // For sleep
#include <fcntl.h>  // For file control options

typedef enum
{
    IDLE,    // 空闲
    PLAYING, // 播放中
    PAUSED,  // 暂停
    STOPPING // 正在停止
}player_state_t;

// 音频缓冲区配置
//总容量：256 * 4096 B = 1048576 B ≈ 1 MB (可以缓冲约 5.8 秒)
#define BUFFER_SIZE 256  // 缓冲区块数 
#define CHUNK_SIZE 4096 // 每块大小(字节)

typedef struct
{
    char data[BUFFER_SIZE][CHUNK_SIZE]; // 音频数据
    int read_pos;  // 读位置
    int write_pos; // 写位置
    int count;     // 当前数据块数量

    // 线程相关的同步机制
    pthread_mutex_t mutex_lock;//互斥量，用于保护读写位置和计数字段
    sem_t empty_chunk;         //信号量，空闲块数量(生产者关注)
    sem_t full_chunk;          //信号量，数据块数量(消费者关注)

    // 控制标志 文件是否读完
    int is_end; // 0 - 未结束, 1 - 已结束
} audio_buffer_t;

// 播放器上下文
typedef struct {
    char current_file[512];              // 当前播放文件
    player_state_t state;                // 播放状态

    pthread_t decode_tid;                // 解码线程ID
    pthread_t output_tid;                // 输出线程ID

    pthread_cond_t pause_cond;           // 暂停条件变量
    pthread_mutex_t state_mutex;         // 状态锁

    audio_buffer_t buffer;               // 共享缓冲区
} player_context_t;

// 缓冲区操作函数
void buffer_init(audio_buffer_t *buf);
void buffer_destroy(audio_buffer_t *buf);
int buffer_write(audio_buffer_t *buf, const char *data, int size);
int buffer_read(audio_buffer_t *buf, char *data, int size);

// 播放器操作函数
void player_init(player_context_t *ctx);
void player_cleanup(player_context_t *ctx);
int player_start(player_context_t *ctx, const char *filepath);
void player_stop(player_context_t *ctx);
void player_pause(player_context_t *ctx);
void player_resume(player_context_t *ctx);

// 线程函数
void *decode_thread(void *arg);
void *output_thread(void *arg);

#endif
