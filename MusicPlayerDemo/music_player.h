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



#endif
