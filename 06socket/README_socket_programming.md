# TCP Socket 编程学习指南

## 📚 目录

1. [TCP 通信流程概览](#tcp-通信流程概览)
2. [服务端流程详解](#服务端流程详解)
3. [客户端流程详解](#客户端流程详解)
4. [关键系统调用说明](#关键系统调用说明)
5. [代码逐步解析](#代码逐步解析)
6. [重要概念与注意事项](#重要概念与注意事项)
7. [常见问题与调试技巧](#常见问题与调试技巧)

---

## TCP 通信流程概览

### 🔄 整体流程图

```
服务端 (Server)                          客户端 (Client)
    |                                        |
    | 1. socket()  创建监听套接字             | 1. socket()  创建套接字
    |                                        |
    | 2. bind()    绑定IP和端口               | 2. bind()    (可选，一般省略)
    |                                        |
    | 3. listen()  开始监听                   |
    |                                        |
    | 4. accept()  阻塞等待连接               | 3. connect() 发起连接请求
    |           ←------- TCP 三次握手 -----→  |
    |                                        |
    | accept() 返回新的连接套接字              | connect() 连接成功
    |                                        |
    | 5. read()/recv()   ←----数据----   send()/write()
    |                                        |
    | 6. send()/write()  ----数据--→    read()/recv()
    |                                        |
    | 7. close()                            | 4. close()
    |           ←------- TCP 四次挥手 -----→  |
    └────────────────────────────────────────┘
```

### 🎯 核心区别

| 步骤     | 服务端                          | 客户端                          |
| -------- | ------------------------------- | ------------------------------- |
| **创建** | socket()                        | socket()                        |
| **绑定** | **必须 bind()** 到指定端口      | **通常不需要**（系统自动分配）  |
| **监听** | **必须 listen()** 变为被动模式  | **不需要**                      |
| **连接** | **accept()** 等待并接受连接     | **connect()** 主动发起连接      |
| **通信** | read()/write() 或 recv()/send() | read()/write() 或 recv()/send() |
| **关闭** | close() 两个 fd（监听+连接）    | close() 一个 fd                 |

---

## 服务端流程详解

### 📝 完整步骤（对应 `03single_connect_server.c`）

#### 步骤 1: 创建套接字

```c
int sockFd = socket(AF_INET, SOCK_STREAM, 0);
```

- **AF_INET**: 使用 IPv4 协议
- **SOCK_STREAM**: TCP 流式套接字（可靠、有序、面向连接）
- **0**: 使用默认协议（TCP）
- **返回值**: 文件描述符（类似文件操作），失败返回 -1

#### 步骤 2: 填写服务器地址信息

```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;                    // IPv4
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);     // 监听所有网卡 (0.0.0.0)
server_addr.sin_port = htons(6666);                  // 端口 6666
```

- **INADDR_ANY (0.0.0.0)**: 监听本机所有网络接口
- **htonl/htons**: 主机字节序转网络字节序（大端）

#### 步骤 3: 绑定地址到套接字

```c
int result = bind(sockFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

- **作用**: 告诉操作系统"我要占用这个 IP:端口"
- **服务器必须 bind**，否则客户端无法找到服务器的端口
- **常见错误**:
  - `EADDRINUSE`: 端口已被占用
  - `EACCES`: 权限不足（小于 1024 的端口需要 root）

#### 步骤 4: 开始监听

```c
int result = listen(sockFd, 128);
```

- **作用**: 将套接字从"主动"模式转为"被动"模式
- **128**: backlog，已完成三次握手但未 accept 的最大连接数
- **不做实际连接**，只是标记为"可接受连接"

#### 步骤 5: 接受客户端连接

```c
struct sockaddr_in client_addr;
socklen_t client_addr_len = sizeof(client_addr);
int clientFd = accept(sockFd, (struct sockaddr *)&client_addr, &client_addr_len);
```

- **阻塞**: 默认会阻塞直到有客户端连接
- **返回值**: 新的套接字描述符，专门用于与该客户端通信
- **client_addr**: 会被填充客户端的 IP 和端口信息
- **重要**: `sockFd` 仍用于监听，`clientFd` 用于与客户端通信

#### 步骤 6: 通信（收发数据）

```c
// 读取数据
ssize_t count = read(clientFd, buffer, 1024);

// 发送数据
ssize_t count = send(clientFd, buffer, len, 0);
```

#### 步骤 7: 关闭连接

```c
close(clientFd);  // 关闭与客户端的连接
close(sockFd);    // 关闭监听套接字
```

---

## 客户端流程详解

### 📝 完整步骤（对应 `04single_connect_client.c`）

#### 步骤 1: 创建套接字

```c
int clientFd = socket(AF_INET, SOCK_STREAM, 0);
```

- 与服务端相同

#### 步骤 2: 填写服务器地址（要连接的目标）

```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);  // 服务器IP
server_addr.sin_port = htons(6666);                       // 服务器端口
```

- **127.0.0.1**: 本机回环地址（测试用）
- **必须是服务器真实的可达 IP**，不能随便写

#### 步骤 3: 绑定本地地址（可选，通常省略）

```c
// 一般不需要！系统会自动分配
// 如果你有特殊需求（固定源端口/多网卡选择），才手动 bind
struct sockaddr_in client_addr;
client_addr.sin_family = AF_INET;
client_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 让系统选择
client_addr.sin_port = htons(0);                   // 0 = 系统自动分配端口
bind(clientFd, (struct sockaddr *)&client_addr, sizeof(client_addr));
```

- **大多数情况下可以省略整个 bind 步骤**
- 系统会在 connect 时自动分配临时端口（ephemeral port，通常 32768-61000）

#### 步骤 4: 连接服务器

```c
int result = connect(clientFd, (struct sockaddr *)&server_addr, sizeof(server_addr));
```

- **主动发起 TCP 三次握手**
- **阻塞**: 默认会阻塞直到连接成功或失败
- **常见错误**:
  - `ECONNREFUSED`: 目标端口没有监听
  - `ETIMEDOUT`: 连接超时
  - `ENETUNREACH`: 网络不可达

#### 步骤 5: 通信

```c
// 发送数据
send(clientFd, buffer, len, 0);

// 接收数据
recv(clientFd, buffer, 1024, 0);
```

#### 步骤 6: 关闭连接

```c
close(clientFd);
```

---

## 关键系统调用说明

### 🔧 socket()

```c
int socket(int domain, int type, int protocol);
```

- **domain**: `AF_INET` (IPv4) 或 `AF_INET6` (IPv6)
- **type**: `SOCK_STREAM` (TCP) 或 `SOCK_DGRAM` (UDP)
- **protocol**: 通常填 0（自动选择）
- **返回**: 文件描述符（>=0）或 -1（失败）

### 🔧 bind()

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- **作用**: 将套接字绑定到指定的 IP 地址和端口
- **服务器**: 必须调用
- **客户端**: 通常不调用（系统自动分配）

### 🔧 listen()

```c
int listen(int sockfd, int backlog);
```

- **仅服务器需要**
- **backlog**: 等待队列最大长度（已完成三次握手但未 accept 的连接）
- **不产生实际连接**，只是标记套接字为"被动监听"

### 🔧 accept()

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- **仅服务器需要**
- **阻塞等待**客户端连接
- **返回**: 新的套接字描述符（用于与该客户端通信）
- **重要**: 原来的 `sockfd` 继续用于监听，新的 fd 用于通信

### 🔧 connect()

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

- **仅客户端需要**
- **发起 TCP 三次握手**
- **阻塞直到成功或失败**

### 🔧 send() / recv()

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

- **flags**: 通常填 0
- **返回值**:
  - `> 0`: 实际发送/接收的字节数
  - `0`: 对端关闭连接（recv 时）
  - `-1`: 错误

### 🔧 shutdown()

```c
int shutdown(int sockfd, int how);
```

- **SHUT_RD**: 关闭读端
- **SHUT_WR**: 关闭写端（常用，告诉对方"我不再发送数据"）
- **SHUT_RDWR**: 关闭读写
- **与 close() 区别**: shutdown 可以半关闭，close 完全关闭

---

## 代码逐步解析

### 🖥️ 服务端代码要点（`03single_connect_server.c`）

#### 1. 多线程处理通信

```c
pthread_t pid_read, pid_write;
pthread_create(&pid_read, NULL, read_from_client, (void *)&clientFd);
pthread_create(&pid_write, NULL, write_to_client, (void *)&clientFd);
```

- **两个线程**:
  - `read_from_client`: 从客户端读取并打印到控制台
  - `write_to_client`: 从控制台读取并发送给客户端
- **实现双向通信**: 同时收发消息

#### 2. 读取线程

```c
void *read_from_client(void *arg) {
    int clientFd = *(int *)arg;
    char *read_buf = (char *)malloc(1024);
    ssize_t count = 0;

    while (count = read(clientFd, read_buf, 1024)) {
        if (count < 0) {
            perror("recv");
        }
        fputs(read_buf, stdout);  // 打印到控制台
    }
    // count == 0 表示客户端关闭连接
    printf("客户端请求关闭连接\n");
    free(read_buf);
    return NULL;
}
```

#### 3. 写入线程

```c
void *write_to_client(void *arg) {
    int clientFd = *(int *)arg;
    char *write_buf = (char *)malloc(1024);

    while (fgets(write_buf, 1024, stdin) != NULL) {  // 从控制台读取
        size_t len = strlen(write_buf);
        send(clientFd, write_buf, len, 0);           // 发送给客户端
    }
    // Ctrl+D 或 EOF 时退出循环
    shutdown(clientFd, SHUT_WR);  // 告诉客户端"我不再发送"
    free(write_buf);
    return NULL;
}
```

### 💻 客户端代码要点（`04single_connect_client.c`）

#### 结构基本相同

- 也是两个线程：
  - `read_from_server`: 接收服务器消息
  - `write_to_server`: 发送消息到服务器
- **区别**: 使用 `connect()` 而不是 `accept()`

---

## 重要概念与注意事项

### 🔑 关键概念

#### 1. 套接字类型

| 类型                      | 说明                                 |
| ------------------------- | ------------------------------------ |
| **监听套接字 (sockFd)**   | 服务器用于监听，只调用 listen/accept |
| **连接套接字 (clientFd)** | accept 返回，用于实际通信            |

#### 2. 字节序转换

```c
// 主机字节序 → 网络字节序（大端）
htons(port)    // host to network short (16位端口)
htonl(ip)      // host to network long (32位IP)

// 网络字节序 → 主机字节序
ntohs(port)    // network to host short
ntohl(ip)      // network to host long
```

- **为什么**: 不同 CPU 架构字节序不同（x86 是小端，网络统一用大端）
- **规则**: 存入结构体用 `hton*`，打印/使用时用 `ntoh*`

#### 3. IP 地址转换

```c
// 文本 → 二进制（推荐）
inet_pton(AF_INET, "192.168.1.1", &addr.sin_addr);

// 二进制 → 文本（推荐）
char ip[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

// 旧方法（不推荐）
inet_addr("192.168.1.1");           // 返回值无法区分错误
inet_ntoa(addr.sin_addr);            // 非线程安全
```

#### 4. INADDR_ANY

```c
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0
```

- **含义**: 监听本机所有网络接口
- **适用**: 服务器通常用这个，允许从任何网卡接收连接
- **区别**: 如果只绑定 `127.0.0.1`，外部机器无法连接

### ⚠️ 常见错误与注意事项

#### 1. accept() 参数传错

```c
// ❌ 错误（会覆盖 server_addr）
accept(sockFd, (struct sockaddr *)&server_addr, &len);

// ✅ 正确（填充 client_addr）
accept(sockFd, (struct sockaddr *)&client_addr, &len);
```

#### 2. bind 失败：地址已被占用

```c
// 解决方案：设置 SO_REUSEADDR
int opt = 1;
setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
bind(sockFd, ...);
```

- **作用**: 允许快速重启服务器（避免 TIME_WAIT 状态占用端口）

#### 3. 客户端 bind 失败

```c
// 如果本机没有 192.168.0.101，会失败
inet_pton(AF_INET, "192.168.0.101", &client_addr.sin_addr);
bind(clientFd, ...);  // ❌ EADDRNOTAVAIL
```

- **解决**: 客户端通常不需要 bind，或用 `INADDR_ANY`

#### 4. 资源泄漏

```c
// ❌ 错误：未关闭套接字
if (error) {
    return -1;  // 泄漏 sockFd/clientFd
}

// ✅ 正确：先关闭再返回
if (error) {
    close(sockFd);
    close(clientFd);
    return -1;
}
```

#### 5. 多线程传参问题

```c
// ❌ 危险：传递栈变量地址
int fd = accept(...);
pthread_create(&tid, NULL, thread_func, &fd);  // fd 可能被覆盖

// ✅ 方案1：动态分配
int *fd_ptr = malloc(sizeof(int));
*fd_ptr = accept(...);
pthread_create(&tid, NULL, thread_func, fd_ptr);
// 线程内 free(fd_ptr)

// ✅ 方案2：确保变量生命周期足够长（本例中 clientFd 在 main 里定义，安全）
```

#### 6. read/recv 返回值处理

```c
ssize_t count = read(fd, buf, size);
if (count > 0) {
    // 成功读取 count 字节
} else if (count == 0) {
    // 对端关闭连接
} else {  // count < 0
    if (errno == EINTR) {
        // 被信号中断，重试
    } else {
        perror("read");  // 真正的错误
    }
}
```

---

## 常见问题与调试技巧

### ❓ 常见问题

#### Q1: 客户端 connect 失败 "Connection refused"

- **原因**: 服务器未启动或端口不对
- **检查**:
  ```bash
  # 查看端口是否在监听
  ss -lntp | grep 6666
  lsof -i :6666
  ```

#### Q2: bind 失败 "Address already in use"

- **原因**: 端口被占用（可能是之前的进程未完全关闭）
- **解决**:
  ```bash
  # 查找占用进程
  lsof -i :6666
  # 杀掉进程
  kill -9 <PID>
  ```
  或在代码中设置 `SO_REUSEADDR`

#### Q3: 服务器只能本机连接，外部无法连接

- **原因**: 服务器绑定到 `127.0.0.1` 而不是 `0.0.0.0`
- **解决**: 使用 `INADDR_ANY` (0.0.0.0)
- **还要检查**: 防火墙是否允许该端口

#### Q4: 程序无响应/假死

- **可能原因**:
  - accept/read/recv 阻塞等待
  - 死锁（多线程）
  - 对端未发送数据
- **调试**: 使用 `strace -p <PID>` 查看系统调用

#### Q5: 只能处理一个客户端

- **原因**: accept 后没有循环或创建新线程/进程
- **解决**:
  ```c
  while (1) {
      int clientFd = accept(...);
      // 方案1: fork 子进程处理
      // 方案2: 创建新线程处理
      // 方案3: 使用 epoll/select 多路复用
  }
  ```

### 🛠️ 调试技巧

#### 1. 使用 netcat 测试

```bash
# 测试服务器
nc 127.0.0.1 6666

# 或 telnet
telnet 127.0.0.1 6666
```

#### 2. 抓包分析

```bash
# 抓取本地回环接口
sudo tcpdump -i lo port 6666 -A

# 查看 TCP 握手
sudo tcpdump -i lo port 6666 -nn
```

#### 3. 查看网络连接状态

```bash
# 查看所有 TCP 连接
ss -tan

# 查看监听端口
ss -lntp

# 查看特定端口
netstat -an | grep 6666
```

#### 4. 编译时启用调试信息

```bash
gcc -g -Wall -pthread 03single_connect_server.c -o server
gcc -g -Wall -pthread 04single_connect_client.c -o client
```

#### 5. 使用 gdb 调试

```bash
gdb ./server
(gdb) break accept
(gdb) run
(gdb) print clientFd
```

---

## 🎓 学习建议

### 循序渐进的学习路径

1. **理解基础流程** ✅

   - socket → bind → listen → accept (服务器)
   - socket → connect (客户端)

2. **掌握字节序和地址转换**

   - htons/htonl/ntohs/ntohl
   - inet_pton/inet_ntop

3. **理解阻塞与非阻塞**

   - fcntl 设置 O_NONBLOCK
   - select/poll/epoll 多路复用

4. **多客户端处理**

   - 多进程（fork）
   - 多线程（pthread）
   - 事件驱动（epoll）

5. **错误处理和健壮性**

   - errno 处理
   - 信号处理（SIGPIPE）
   - 超时控制

6. **高级主题**
   - TCP 粘包问题
   - 心跳机制
   - 断线重连
   - 负载均衡

### 📚 推荐实践

1. **修改现有代码**:

   - 添加 SO_REUSEADDR
   - 实现多客户端支持（循环 accept + fork/thread）
   - 添加心跳检测

2. **编写小项目**:

   - 简单聊天室
   - 文件传输工具
   - HTTP 服务器（简化版）

3. **阅读源码**:
   - Redis 网络模块
   - Nginx 事件循环
   - libevent/libev

---

## 📖 参考资料

- **书籍**:

  - 《Unix Network Programming》（UNP）- Stevens
  - 《TCP/IP 详解 卷 1》
  - 《Linux 高性能服务器编程》

- **文档**:

  - `man 2 socket`
  - `man 7 tcp`
  - `man 7 ip`

- **在线资源**:
  - Beej's Guide to Network Programming
  - The Linux Programming Interface

---

## ✨ 总结

**TCP Socket 编程核心流程**:

```
服务器: socket → bind → listen → accept → read/write → close
客户端: socket → connect → write/read → close
```

**关键点**:

1. ✅ 服务器必须 bind + listen，客户端通常不需要 bind
2. ✅ accept 返回新的 fd 用于通信，原 fd 继续监听
3. ✅ 所有地址/端口存入结构体前要转换字节序（htons/htonl）
4. ✅ 打印/使用前要转换回主机字节序（ntohs/ntohl）
5. ✅ 记得关闭所有打开的文件描述符
6. ✅ 多线程时注意传参和资源竞争

**下一步**:

- 运行并测试你的服务器和客户端
- 尝试用 `nc` 或 `telnet` 连接测试
- 逐步添加功能（多客户端、错误处理等）

祝学习顺利！🚀
