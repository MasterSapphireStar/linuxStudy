#ifndef PLAYER_H
#define PLAYER_H

#include <pthread.h>
#include <semaphore.h>

// 播放器状态
typedef enum {
    STATE_IDLE,      // 空闲
    STATE_PLAYING,   // 播放中
    STATE_PAUSED,    // 暂停
    STATE_STOPPING   // 正在停止
} player_state_t;

// 音频缓冲区配置
#define BUFFER_SIZE 10           // 缓冲区块数
#define CHUNK_SIZE 4096          // 每块大小(字节)

// 共享缓冲区结构（环形缓冲区）
typedef struct {
    char data[BUFFER_SIZE][CHUNK_SIZE];  // 音频数据
    int read_pos;                        // 读位置
    int write_pos;                       // 写位置
    int count;                           // 当前数据块数量

    // 同步机制
    pthread_mutex_t mutex;               // 互斥锁
    sem_t empty;                         // 空槽位信号量
    sem_t full;                          // 数据信号量

    // 控制标志
    int is_end;                          // 文件是否读完
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

// 函数声明
void player_init(player_context_t *ctx);
void player_cleanup(player_context_t *ctx);
int player_start(player_context_t *ctx, const char *filepath);
void player_stop(player_context_t *ctx);
void player_pause(player_context_t *ctx);
void player_resume(player_context_t *ctx);

// 线程函数
void *decode_thread(void *arg);
void *output_thread(void *arg);

// 缓冲区操作
void buffer_init(audio_buffer_t *buf);
void buffer_destroy(audio_buffer_t *buf);
int buffer_write(audio_buffer_t *buf, const char *data, int size);
int buffer_read(audio_buffer_t *buf, char *data, int size);

#endif
