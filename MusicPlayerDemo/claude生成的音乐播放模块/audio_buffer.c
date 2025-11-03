#include "player.h"
#include <string.h>
#include <stdio.h>

// 初始化缓冲区
void buffer_init(audio_buffer_t *buf) {
    buf->read_pos = 0;
    buf->write_pos = 0;
    buf->count = 0;
    buf->is_end = 0;

    // 初始化互斥锁
    pthread_mutex_init(&buf->mutex, NULL);

    // 初始化信号量
    sem_init(&buf->empty, 0, BUFFER_SIZE);  // 初始有BUFFER_SIZE个空位
    sem_init(&buf->full, 0, 0);             // 初始没有数据

    printf("✓ Audio buffer initialized (size: %d blocks)\n", BUFFER_SIZE);
}

// 销毁缓冲区
void buffer_destroy(audio_buffer_t *buf) {
    pthread_mutex_destroy(&buf->mutex);
    sem_destroy(&buf->empty);
    sem_destroy(&buf->full);
}

// 写入数据到缓冲区（生产者）
int buffer_write(audio_buffer_t *buf, const char *data, int size) {
    if (size > CHUNK_SIZE) {
        fprintf(stderr, "Error: Data size exceeds chunk size\n");
        return -1;
    }

    // 等待空槽位
    sem_wait(&buf->empty);

    // 加锁
    pthread_mutex_lock(&buf->mutex);

    // 写入数据
    memcpy(buf->data[buf->write_pos], data, size);
    buf->write_pos = (buf->write_pos + 1) % BUFFER_SIZE;
    buf->count++;

    // 解锁
    pthread_mutex_unlock(&buf->mutex);

    // 通知有数据
    sem_post(&buf->full);

    return size;
}

// 从缓冲区读取数据（消费者）
int buffer_read(audio_buffer_t *buf, char *data, int size) {
    // 等待数据
    sem_wait(&buf->full);

    // 加锁
    pthread_mutex_lock(&buf->mutex);

    // 读取数据
    int actual_size = (size < CHUNK_SIZE) ? size : CHUNK_SIZE;
    memcpy(data, buf->data[buf->read_pos], actual_size);
    buf->read_pos = (buf->read_pos + 1) % BUFFER_SIZE;
    buf->count--;

    // 解锁
    pthread_mutex_unlock(&buf->mutex);

    // 通知有空位
    sem_post(&buf->empty);

    return actual_size;
}
