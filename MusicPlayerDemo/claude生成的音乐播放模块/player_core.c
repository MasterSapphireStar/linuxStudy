#include "player.h"
#include <stdio.h>
#include <string.h>

// 初始化播放器
void player_init(player_context_t *ctx) {
    memset(ctx, 0, sizeof(player_context_t));

    ctx->state = STATE_IDLE;

    // 初始化缓冲区
    buffer_init(&ctx->buffer);

    // 初始化条件变量和互斥锁
    pthread_cond_init(&ctx->pause_cond, NULL);
    pthread_mutex_init(&ctx->state_mutex, NULL);

    printf("✓ Player initialized\n");
}

// 清理播放器
void player_cleanup(player_context_t *ctx) {
    // 停止播放
    if (ctx->state != STATE_IDLE) {
        player_stop(ctx);
    }

    // 销毁缓冲区
    buffer_destroy(&ctx->buffer);

    // 销毁同步对象
    pthread_cond_destroy(&ctx->pause_cond);
    pthread_mutex_destroy(&ctx->state_mutex);

    printf("✓ Player cleaned up\n");
}

// 开始播放
int player_start(player_context_t *ctx, const char *filepath) {
    // 如果正在播放，先停止
    if (ctx->state != STATE_IDLE) {
        player_stop(ctx);
    }

    // 保存文件路径
    strncpy(ctx->current_file, filepath, sizeof(ctx->current_file) - 1);

    // 重置缓冲区
    pthread_mutex_lock(&ctx->buffer.mutex);
    ctx->buffer.read_pos = 0;
    ctx->buffer.write_pos = 0;
    ctx->buffer.count = 0;
    ctx->buffer.is_end = 0;
    pthread_mutex_unlock(&ctx->buffer.mutex);

    // 设置状态
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = STATE_PLAYING;
    pthread_mutex_unlock(&ctx->state_mutex);

    // 创建线程
    if (pthread_create(&ctx->decode_tid, NULL, decode_thread, ctx) != 0) {
        perror("Failed to create decode thread");
        ctx->state = STATE_IDLE;
        return -1;
    }

    if (pthread_create(&ctx->output_tid, NULL, output_thread, ctx) != 0) {
        perror("Failed to create output thread");
        pthread_cancel(ctx->decode_tid);
        ctx->state = STATE_IDLE;
        return -1;
    }

    printf("▶ Playing: %s\n", filepath);
    return 0;
}

// 停止播放
void player_stop(player_context_t *ctx) {
    if (ctx->state == STATE_IDLE) {
        return;
    }

    printf("⏹ Stopping playback...\n");

    // 设置停止状态
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = STATE_STOPPING;
    pthread_cond_broadcast(&ctx->pause_cond);  // 唤醒可能暂停的线程
    pthread_mutex_unlock(&ctx->state_mutex);

    // 等待线程结束
    pthread_join(ctx->decode_tid, NULL);
    pthread_join(ctx->output_tid, NULL);

    // 恢复空闲状态
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = STATE_IDLE;
    pthread_mutex_unlock(&ctx->state_mutex);

    printf("✓ Playback stopped\n");
}

// 暂停播放
void player_pause(player_context_t *ctx) {
    pthread_mutex_lock(&ctx->state_mutex);
    if (ctx->state == STATE_PLAYING) {
        ctx->state = STATE_PAUSED;
        printf("⏸ Paused\n");
    }
    pthread_mutex_unlock(&ctx->state_mutex);
}

// 继续播放
void player_resume(player_context_t *ctx) {
    pthread_mutex_lock(&ctx->state_mutex);
    if (ctx->state == STATE_PAUSED) {
        ctx->state = STATE_PLAYING;
        pthread_cond_broadcast(&ctx->pause_cond);
        printf("▶ Resumed\n");
    }
    pthread_mutex_unlock(&ctx->state_mutex);
}
