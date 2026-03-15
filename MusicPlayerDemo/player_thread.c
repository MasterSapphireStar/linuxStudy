#include "music_player.h"

// ========== 解码线程（生产者）==========
void *decode_thread(void *arg)
{
    player_context_t *ctx = (player_context_t *)arg;

    printf("→ Decode thread started for: %s\n", ctx->current_file);

    // 打开音频文件
    FILE *fp = fopen(ctx->current_file, "rb");
    if (!fp)
    {
        perror("Failed to open audio file");
        ctx->buffer.is_end = 1;
        pthread_exit(NULL);
    }

    char chunk[CHUNK_SIZE];
    int bytes_read;

    // 循环读取文件并写入缓冲区
    while ((bytes_read = fread(chunk, 1, CHUNK_SIZE, fp)) > 0)
    {
        // 检查是否需要停止
        pthread_mutex_lock(&ctx->state_mutex);
        if (ctx->state == STOPPING)
        {
            pthread_mutex_unlock(&ctx->state_mutex);
            break;
        }
        pthread_mutex_unlock(&ctx->state_mutex);

        // 写入缓冲区
        buffer_write(&ctx->buffer, chunk, bytes_read);

        // 模拟解码延迟（可选）
        // usleep(1000);
    }

    fclose(fp);

    // 标记文件读取完毕
    pthread_mutex_lock(&ctx->buffer.mutex_lock);
    ctx->buffer.is_end = 1;
    pthread_mutex_unlock(&ctx->buffer.mutex_lock);

    printf("✓ Decode thread finished\n");
    pthread_exit(NULL);
}

// ========== 输出线程（消费者）==========
void *output_thread(void *arg)
{
    player_context_t *ctx = (player_context_t *)arg;

    printf("→ Output thread started\n");

    char chunk[CHUNK_SIZE];

    while (1)
    {
        // 检查暂停状态
        pthread_mutex_lock(&ctx->state_mutex);
        while (ctx->state == PAUSED)
        {
            printf("⏸ Paused...\n");
            pthread_cond_wait(&ctx->pause_cond, &ctx->state_mutex);
            printf("▶ Resumed\n");
        }

        // 检查停止状态
        if (ctx->state == STOPPING) {
            pthread_mutex_unlock(&ctx->state_mutex);
            break;
        }
        pthread_mutex_unlock(&ctx->state_mutex);

        // 检查是否播放完毕
        pthread_mutex_lock(&ctx->buffer.mutex_lock);
        int is_end = ctx->buffer.is_end;
        int count = ctx->buffer.count;
        pthread_mutex_unlock(&ctx->buffer.mutex_lock);

        if (is_end && count == 0)
        {
            printf("✓ Playback finished\n");
            break;
        }

        // 从缓冲区读取数据
        if (count > 0)
        {
            buffer_read(&ctx->buffer, chunk, CHUNK_SIZE);

            // ========== 这里是音频输出的位置 ==========
            // TODO: 调用实际的音频输出API (ALSA/OSS/PulseAudio)
            // 目前用sleep模拟播放延迟
            usleep(20000);  // 模拟20ms播放时间

            // 简单的播放进度指示
            static int dots = 0;
            if (++dots % 10 == 0)
            {
                printf("♪");
                fflush(stdout);
            }
        }
    }

    printf("\n✓ Output thread finished\n");
    pthread_exit(NULL);
}

