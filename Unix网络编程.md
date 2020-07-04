# Unix网络编程

## Socket

socket其实对应的是内核中的一片缓冲区，Socket操作其实就是文件IO操作

### CS模式

<font color=red>TCP Server</font>

- 创建套接字

  - int lfd = socket

- 绑定本地IP和端口

  - struct sockaddr_in serv;
  - serv.port = htons(port);
  - serv.IP = htonl(INARRR_ANY);

  - bind(lfd,&serv,sizeof(serv));

- 监听

  - listen(lfd,128);

- 等待连接请求

  - struct sockaddr_in client;
  - int len = sizeof(client);

  - int cfd = accept(lfd,&client,&len);
    - cfd用于通信描述符

- 通信

  - 接收数据：read/recv
  - 发送数据：write/send

- 关闭

  - close(lfd);
  - close(cfd);

<font color=red>TCP Client</font>

- 创建套接字
  - int fd = socket
- 连接服务器
  - struct sockaddr_in server;
  - server.port
  - server.ip = (int) ???
  - server.family
  - connect(fd,&server,sizeof(server));
- 通信
  - 接收数据：read/recv
  - 发送数据：write/send
- 断开连接
  - close(fd);



## TCP三次握手

<font color=red>第一次握手</font>

- 客户端
  - 携带标志位：SYN
  - 随机产生32位序号
    - 可以携带数据
- 服务器
  - 检测SYN值是否为1

<font color=red>第二次握手</font>

- 服务器
  - ACK标志位 ＋　确认序号
    - 客户端随机序号＋１
  - 发起一个连接请求
    - SYN+32随机序号
- 客户端
  - 检测标志位：１
  - 校验：确认序号正确

<font color=red>第三次握手</font>

- 客户端
  - 发送确认数据包
    - ACK+确认序号
      - 服务器的随机序号＋１
- 服务器
  - 检测：ACK是否为１
  - 校验：确认序号是否正确



## TCP四次挥手

客户端或者服务器端任何一端主动断开连接都可以

<font color=red>第一次挥手</font>

- 客户端
  - 发送断开连接的请求
    - FIN+序号
    - ACK+序号
- 服务器
  - 检测FIN值是否为１
  - 检测ACK值是否正确

<font color=red>第二次挥手</font>

- 服务器
  - 给client确认数据包
    - ACK+确认序号
      - FIN对应的序号+1+携带数据大小
- 客户端
  - 检测;ACK值
  - 检测确认序号

<font color=red>第三次挥手</font>

- 服务器
  - FIN+序号
  - ACK+序号

<font color=red>第四次挥手</font>

- 客户端
  - ACK+序号





## 多进程并发服务器

使用多进程的方式，解决服务器处理多连接的问题：

1. 共享
   - 读时共享，写时复制
   - 文件描述符
   - 内存映射区--mmap
2. 父进程的角色是什么？
   - 等待接受客户端连接--accept
     - 有连接请求
       - 创建一个子进程fork()
   - 将通信的文件描述符关闭
3. 子进程的角色是什么？
   - 通信
     - 使用accept返回值-fd
   - 关掉监听的文件描述符
4. 创建的进程的个数有限制么？
   - 受硬件限制
   - 文件描述符默认也是有上限的
5. 子进程资源回收
   - wait/waitpid
   - 信号回收
     - 信号捕捉
       - signal
       - sigaction--常用
     - 捕捉信号sigchild