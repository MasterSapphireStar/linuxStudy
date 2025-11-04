#include "music_player.h"

// 初始化播放器
void player_init(player_context_t *ctx)
{
    //使用 memset 函数将整个 player_context_t 结构体占用的内存空间全部设置为 $0$。
    memset(ctx, 0, sizeof(player_context_t));

    ctx->state = IDLE;

    // 初始化缓冲区
    buffer_init(&ctx->buffer);

    // 初始化条件变量和互斥锁
    //条件变量不是用来保护共享数据本身的（这是互斥锁的工作），而是用来帮助线程在某个条件未满足时高效地等待。
    pthread_cond_init(&ctx->pause_cond, NULL);
    pthread_mutex_init(&ctx->state_mutex, NULL);

    printf("✓ Player initialized\n");
}

// 清理播放器
void player_cleanup(player_context_t *ctx)
{
    if (ctx->state != IDLE)
    {
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
int player_start(player_context_t *ctx, const char *filepath)
{
    // 如果正在播放，先停止
    if (ctx->state != IDLE)
    {
        player_stop(ctx);
    }

    // 保存文件路径
    /**
     * @brief 复制字符串到目标缓冲区
     * char *strncpy (char *__restrict __dest,const char *__restrict __src, size_t __n)
        * 功能：将源字符串 __src 复制到目标缓冲区 __dest 中，最多复制 __n - 1 个字符，并在末尾添加字符串结束符 \0。
     */
     //将 filepath 复制到 ctx->current_file 中，确保不会超过缓冲区大小，防止溢出
    strncpy(ctx->current_file, filepath, sizeof(ctx->current_file) - 1);

    //重置缓冲区：访问缓冲区共享数据之前，必须使用其对应的互斥锁 ctx->buffer.mutex_lock 进行锁定和解锁，以确保重置操作的原子性和线程安全。
    pthread_mutex_lock(&ctx->buffer.mutex_lock);
    ctx->buffer.read_pos = 0;
    ctx->buffer.write_pos = 0;
    ctx->buffer.count = 0;
    ctx->buffer.is_end = 0;
    pthread_mutex_unlock(&ctx->buffer.mutex_lock);

    //同样，在修改播放状态 ctx->state 时，使用 ctx->state_mutex 进行保护，将其设置为 PLAYING
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = PLAYING;
    pthread_mutex_unlock(&ctx->state_mutex);

    // 创建线程
    /**
     * @brief 创建解码和输出线程
     * int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        * 功能：创建一个新的线程，执行指定的函数 start_routine，并传递参数 arg 给该函数。
        * @param thread 指向 pthread_t 变量的指针，用于存储新创建线程的 ID。
        * @param attr 指向线程属性对象的指针，通常为 NULL，表示使用默认属性。
        * @param start_routine 指向线程将要执行的函数的指针，该函数必须符合特定的签名（接受一个 void* 参数并返回一个 void*）。
        * @param arg 传递给 start_routine 函数的参数，可以是任何类型的数据，通过 void* 指针传递。
        * @return int 返回 0 表示成功创建线程，非零值表示失败，并设置相应的错误码。
     */
    if (pthread_create(&ctx->decode_tid, NULL, decode_thread, ctx) != 0)
    {
        perror("Failed to create decode thread");
        ctx->state = IDLE;
        return -1;
    }

    if (pthread_create(&ctx->output_tid, NULL, output_thread, ctx) != 0)
    {
        perror("Failed to create output thread");
        pthread_cancel(ctx->decode_tid);
        ctx->state = IDLE;
        return -1;
    }

    printf("▶ Playing: %s\n", filepath);
    return 0;
}

// 停止播放
void player_stop(player_context_t *ctx)
{
    if (ctx->state == IDLE) {
        return;
    }

    printf("⏹ Stopping playback...\n");

    // 设置停止状态
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = STOPPING;
    //向所有正在等待 ctx->pause_cond 条件变量的线程发送广播信号。这是至关重要的一步，
    //因为它会唤醒任何处于 PAUSED 状态的线程，确保它们能检查到 STOPPING 状态并退出，避免死锁。
    pthread_cond_broadcast(&ctx->pause_cond);  // 唤醒可能暂停的线程
    pthread_mutex_unlock(&ctx->state_mutex);

    // 等待线程结束
    /**
     * @brief 等待解码和输出线程结束
     * int pthread_join(pthread_t thread, void **retval);
     * 阻塞当前线程（即player_stop函数所在的线程），直到目标线程（decode_tid 或 output_tid）终止为止。
     */
    pthread_join(ctx->decode_tid, NULL);
    pthread_join(ctx->output_tid, NULL);

    // 恢复空闲状态
    pthread_mutex_lock(&ctx->state_mutex);
    ctx->state = IDLE;
    pthread_mutex_unlock(&ctx->state_mutex);

    printf("✓ Playback stopped\n");
}

// 暂停播放
void player_pause(player_context_t *ctx)
{
    pthread_mutex_lock(&ctx->state_mutex);
    if (ctx->state == PLAYING)
    {
        ctx->state = PAUSED;
        printf("⏸ Paused\n");
    }
    pthread_mutex_unlock(&ctx->state_mutex);
}

// 继续播放
void player_resume(player_context_t *ctx)
{
    pthread_mutex_lock(&ctx->state_mutex);
    if (ctx->state == PAUSED)
    {
        ctx->state = PLAYING;
        pthread_cond_broadcast(&ctx->pause_cond);
        printf("▶ Resumed\n");
    }
    pthread_mutex_unlock(&ctx->state_mutex);
}

