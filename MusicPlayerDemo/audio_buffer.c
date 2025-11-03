#include "music_player.h"

// 初始化缓冲区
void buffer_init(audio_buffer_t *buf)
{
    buf->read_pos = 0; //读位置
    buf->write_pos = 0;//写位置
    buf->count = 0;    //当前块数量
    buf->is_end = 0;   //0-文件尚未读取完成

    /**
     * @brief 初始化互斥锁
     * int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
     * @param mutex 指向要初始化的互斥锁的指针
     * @param attr 指向互斥锁属性的指针，通常为NULL，表示使用默认属性
     * @return 成功返回0，失败返回错误码
     */
    pthread_mutex_init(&buf->mutex_lock, NULL);

    /**
     * @brief 初始化信号量
     * int sem_init(sem_t *sem, int pshared, unsigned int value);
     * @param sem 指向信号量的指针
     * @param pshared 如果为0，表示信号量在同一进程的线程间共享；非0表示在不同进程间共享
     * @param value 信号量的初始值
     * @return 成功返回0，失败返回-1并设置errno
     */
    sem_init(&buf->empty_chunk, 0, BUFFER_SIZE);//初始状态有BUFFER_SIZE(所有)个空闲块
    sem_init(&buf->full_chunk, 0, 0); //初始没有数据，第三个值给0

    printf("✓ Audio buffer initialized (size: %d chunks)\n", BUFFER_SIZE);
}

// 摧毁缓冲区
void buffer_destroy(audio_buffer_t *buf)
{
    pthread_mutex_destroy(&buf->mutex_lock);// 销毁互斥锁
    sem_destroy(&buf->empty_chunk);// 销毁信号量
    sem_destroy(&buf->full_chunk); // 销毁信号量
}


/**
 * @brief 写入数据到缓冲区（生产者）
 * @param buf 
 * @param data 
 * @param size 
 * @return int size 写入的字节数，失败返回-1
 */
int buffer_write(audio_buffer_t *buf, const char *data, int size)// 加const是为了防止修改传入的数据
{
    if (size>CHUNK_SIZE)
    {
        //将格式化的文本输出到指定的文件流（FILE *），可以是文件、标准输出或标准错误等,这里使用的是错误输出
        fprintf(stderr, "Error: Input data size exceeds chunk size!\n");
        return -1;
    }
    /**
     * @brief 等待空闲块
     * int sem_wait (sem_t *__sem)
     * @param __sem 指向要等待的信号量的指针
     * @return 成功返回0，失败返回-1并设置errno
     * 空闲块大于0时，空闲块减1并继续执行；否则阻塞等待
     */
    sem_wait(&buf->empty_chunk);
    pthread_mutex_lock(&buf->mutex_lock); //加锁 其他线程无法访问被保护的资源
    /**
     * @brief 将数据写入缓冲区
     * void *memcpy(void *__restrict__ __dest, const void *__restrict__ __src, size_t __n)
     * @param __dest 指向目标内存区域的指针
     * @param __src 指向源内存区域的指针
     * @param __n 要复制的字节数
     * 将源内存区域的前n个字节复制到目标内存区域
     */
    memcpy(buf->data[buf->write_pos], data, size);
    buf->write_pos = (buf->write_pos + 1) % BUFFER_SIZE;//这一行把写指针向前移动一个位置，并在到达缓冲区末尾时mod BUFFER_SIZE=0，于是回到起点。
    buf->count++;//count:当前数据块数量

    pthread_mutex_unlock(&buf->mutex_lock);//解锁 允许其他线程访问被保护的资源

    /**
     * @brief 通知有数据块可用
     * int sem_post (sem_t *__sem)
     * @param __sem 指向要增加的信号量的指针
     * 增加信号量的值，如果有线程在等待该信号量，则唤醒其中一个线程
     */
    sem_post(&buf->full_chunk);
    return size;
}

/**
 * @brief 从缓冲区读取数据（消费者）
 * @param buf 
 * @param data 
 * @param size 
 * @return int actual_size 读取的字节数
 */
int buffer_read(audio_buffer_t *buf, const char *data, int size)
{
    sem_wait(&buf->full_chunk);
    pthread_mutex_lock(&buf->mutex_lock); //加锁 其他线程无法访问被保护的资源

    //读取数据
    int actual_size = (size < BUFFER_SIZE) ? size : BUFFER_SIZE;
    memcpy(data, buf->data[buf->read_pos], size);
    buf->read_pos = (buf->read_pos + 1) % BUFFER_SIZE;
    buf->count--;

    pthread_mutex_unlock(&buf->mutex_lock);//解锁 允许其他线程访问被保护的资源
    //通知有空位
    sem_post(&buf->empty_chunk);
    return actual_size;
}
