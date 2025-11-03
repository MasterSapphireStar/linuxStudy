CC := gcc
CFLAGS := -Wall -g -pthread
LDFLAGS := -lpthread

# 所有目标文件
PLAYER_OBJS := player_core.o audio_thread.o audio_buffer.o music_manager.o
TEST_OBJS := test_player.o $(PLAYER_OBJS)

# 默认目标
all: test_player

# 测试程序
test_player: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 播放器核心
player_core.o: player_core.c player.h
	$(CC) $(CFLAGS) -c $<

audio_thread.o: audio_thread.c player.h
	$(CC) $(CFLAGS) -c $<

audio_buffer.o: audio_buffer.c player.h
	$(CC) $(CFLAGS) -c $<

music_manager.o: music_manager.c music_manager.h
	$(CC) $(CFLAGS) -c $<

test_player.o: test_player.c player.h music_manager.h
	$(CC) $(CFLAGS) -c $<

# 网络服务器（保留你的原有代码）
net_server: net_server.c net_server.h
	$(CC) $(CFLAGS) -o $@ net_server.c $(LDFLAGS)

# 清理
clean:
	rm -f *.o test_player net_server

# 创建音乐目录
music_dir:
	mkdir -p music

.PHONY: all clean music_dir