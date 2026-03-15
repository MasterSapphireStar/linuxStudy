#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "music_manager.h"
#include "music_player.h"
#include "net_server.h"

/* 全局保存网络子进程 PID，便于退出时发送信号 */
static pid_t net_pid = -1;

/*
 * SIGCHLD 信号处理函数
 * 功能：回收已经退出的子进程，避免产生僵尸进程
 * 说明：这里只做简单的非阻塞回收，避免阻塞主事件循环
 */
static void sigchld_handler(int sig)
{
    (void)sig;
    /* Reap any dead children to avoid zombies */
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
        /* 循环回收直到没有可回收的子进程 */
    }
}

/* 去掉字符串末尾的换行符（如果存在） */
static void trim_newline(char *s)
{
    char *p = strchr(s, '\n');
    if (p)
    {
        *p = '\0';
    }
}

/*
 * 程序入口
 * 职责：
 *  1) 尝试加载 playlist.txt，如果失败则扫描 ./music 目录
 *  2) 初始化播放器上下文
 *  3) fork 出网络子进程用于接收远程控制命令
 *  4) 提供交互式命令行（play, next, prev, pause, resume, stop, list, quit）
 */
int main(int argc, char **argv)
{
    /* 使用 signal 注册 SIGCHLD 处理器，避免子进程成为僵尸 */
    signal(SIGCHLD, sigchld_handler);

    /* 1) 加载播放列表：优先读取 playlist.txt，否则扫描 ./music 目录 */
    playlist_t playlist;
    int n = load_playlist(&playlist, "playlist.txt");
    if (n <= 0)
    {
        if (scan_music_dir(&playlist, "./music") <= 0)
        {
            printf("未找到歌曲。请将音频文件放到 ./music 目录，或创建 playlist.txt。\n");
        }
    }

    /* 打印已加载的播放列表，便于用户查询索引 */
    print_playlist(&playlist);

    /* 2) 初始化播放器上下文（缓冲区、同步对象等） */
    player_context_t player;
    player_init(&player);

    /* 3) 创建网络子进程，网络进程负责接收远程命令并转发（当前实现阻塞运行） */
    net_pid = fork();
    if (net_pid < 0)
    {
        perror("fork");
    }
    else if (net_pid == 0)
    {
        /* 子进程：运行网络服务器函数（会阻塞在 accept 或事件循环中） */
        net_server_init();
        /* 当网络服务返回时，子进程退出 */
        _exit(0);
    }
    else
    {
        printf("已启动 net_server (pid=%d)\n", (int)net_pid);
    }

    /* 4) 交互式命令行，供本地手动控制播放器（也可以通过网络命令控制） */
    char line[512];
    printf("输入 'help' 获取可用命令。\n");
    while (1)
    {
        printf("mp> ");
        if (!fgets(line, sizeof(line), stdin))
        {
            break; /* stdin 已关闭或出错，退出循环 */
        }

        trim_newline(line);
        if (line[0] == '\0')
        {
            continue; /* 空行，跳过 */
        }

        /* 帮助信息 */
        if (strcmp(line, "help") == 0)
        {
            printf("命令列表:\n");
            printf("  play <idx>     - 按播放列表索引播放歌曲\n");
            printf("  playfile <path>- 直接播放指定路径的文件\n");
            printf("  next           - 下一首\n");
            printf("  prev           - 上一首\n");
            printf("  pause          - 暂停播放\n");
            printf("  resume         - 继续播放\n");
            printf("  stop           - 停止播放\n");
            printf("  list           - 显示播放列表\n");
            printf("  quit / exit    - 退出程序\n");
            continue;
        }

        /* play 命令：支持索引或路径两种形式 */
        if (strncmp(line, "play ", 5) == 0)
        {
            char *arg = line + 5;
            while (*arg && isspace((unsigned char)*arg))
            {
                arg++;
            }

            if (*arg == '\0')
            {
                printf("用法: play <index>\n");
                continue;
            }

            if (isdigit((unsigned char)arg[0]))
            {
                /* 按索引播放 */
                int idx = atoi(arg);
                if (idx < 0 || idx >= playlist.count)
                {
                    printf("索引越界: %d\n", idx);
                }
                else
                {
                    const char *path = playlist.songs[idx].path;
                    player_start(&player, path);
                }
            }
            else
            {
                /* 将参数视为文件路径直接播放 */
                player_start(&player, arg);
            }

            continue;
        }

        /* 直接播放文件路径 */
        if (strncmp(line, "playfile ", 9) == 0)
        {
            char *path = line + 9;
            while (*path && isspace((unsigned char)*path))
            {
                path++;
            }

            if (*path == '\0')
            {
                printf("用法: playfile <path>\n");
                continue;
            }

            player_start(&player, path);
            continue;
        }

        /* 播放列表控制 */
        if (strcmp(line, "next") == 0)
        {
            char *p = next_song(&playlist);
            if (p)
            {
                player_start(&player, p);
            }

            continue;
        }

        if (strcmp(line, "prev") == 0)
        {
            char *p = prev_song(&playlist);
            if (p)
            {
                player_start(&player, p);
            }

            continue;
        }

        /* 播放控制：暂停/继续/停止 */
        if (strcmp(line, "pause") == 0)
        {
            player_pause(&player);
            continue;
        }

        if (strcmp(line, "resume") == 0)
        {
            player_resume(&player);
            continue;
        }

        if (strcmp(line, "stop") == 0)
        {
            player_stop(&player);
            continue;
        }

        /* 列表显示 */
        if (strcmp(line, "list") == 0)
        {
            print_playlist(&playlist);
            continue;
        }

        /* 退出命令 */
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0)
        {
            break;
        }

        printf("未知命令: %s\n", line);
    }

    /* 退出清理流程：停止播放器，清理资源，终止网络子进程 */
    printf("正在关闭...\n");

    /* 停止播放器（如果在运行）并释放相关资源 */
    player_stop(&player);
    player_cleanup(&player);

    if (net_pid > 0)
    {
        /* 向网络子进程发送中断信号，通知其退出 */
        kill(net_pid, SIGINT);
        /* 等待子进程退出并回收 */
        waitpid(net_pid, NULL, 0);
    }

    printf("再见。\n");
    return 0;
}
