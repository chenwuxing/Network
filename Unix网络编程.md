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
     - 捕捉信号SIGCHLD



## IO操作方式

- 阻塞等待

  - 好处：不占用CPU宝贵的时间片
  - 缺点：同一时刻只能处理一个操作，效率低
  - 改进的方式是采用多进程或者多线程，但浪费系统资源

- 非阻塞，忙轮询

  - 优点：提高了程序的执行效率
  - 缺点：需要占用更多的CPU和系统资源

- IO多路转接技术

  委托他人做事情

  - select

    ```c
    int select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *timeout);
    ```

    - 优点：跨平台
    - 缺点:
      - 每次调用select,都需要把fd集合从用于态拷贝到内核态，这个开销在fd很多时会很大
      - 同时每次调用select都需要在内核遍历传递进来的所有fd，这个开销在fd很多时也很大
      - select支持的文件描述符数量太小了，默认是1024

    <font color=red>select如何实现的？</font>

    -   先构造一张有关文件描述符的列表，将要监听的文件描述符添加到该表中
    -   然后调用一个函数，监听该表中的文件描述符，直到这些描述符表中的一个进行I/O操作时，该函数才返回
        -   该函数为阻塞函数
        -   函数对文件描述符的检测操作是由内核完成的
    -   在返回时，它告诉进程有多少（哪些）描述符要进行IO操作

    

  - poll

    ```c
    int poll(struct pollfd *fd,nfds_t nfds,int timeout);
    /**
    1.pollfd --数组的地址
    2.nfds:数组的最大长度，数组中最后一个使用的元素下标+1
    3.timeout
    		- -1:永久阻塞
    		- 0 ：调用完成立即返回
    		- >0 :等待的时长毫秒
    		
    4.返回值：IO发送变化的文件描述符的个数
    **/
    ```

    

  - epoll

      ```c
    int epoll_create(int size);	/该函数生成一个epoll专用的文件描述符
          size:epoll上能关注的最大描述符数
      
       int epoll_ctl(int epfd,int op,int fd,struct epoll_event *event);	// 用于控制某个epoll文件描述符事件，可以注册、修改、删除
      	epfd:epoll_create生成的epoll专用描述符
           op:
      		（1）EPOLL_CTL_ADD 注册
                 (2）EPOLL_CTL_MOD  修改
                 (3)  EPOLL_CTL_DEL   删除
                  
           fd:关联的文件描述符
           event:告诉内核要监听什么事件
                  
                  
        int epoll_wait(int epfd,struct epoll_event *event,int maxevents,int timeout);	//等待IO事件发生，可以设置阻塞的函数，对应select与poll
           epfd:要检测的句柄
            events:用于回传待处理事件的数组
            maxevents:告诉内核这个events的大小
            timeout:超时时间  
    ```

      <font color=red>epoll的三种模式</font>
    
    -   水平触发模式
    
        -   只要fd对应的缓冲区有数据
        -   epoll_wait返回
        -   返回的次数与发送数据的次数没有关系
        -   epoll默认的工作模式
    
    -   边沿触发模式
    
        -   客户端给服务器端发数据
    
            -   发一次数据server的epoll_wait返回一次
    
            -   不在乎数据是否读完
    
            -   <font color=red>如果读不完，如何全部读出来？</font>
    
                while(recv());
    
                -   数据读完之后recv会阻塞
    
                -   解决阻塞问题
    
                    设置非阻塞-fd
    
    -   边沿非阻塞
    
        -   效率最高
        -   如何设置非阻塞
            -   open
                -   设置flags
                -   必须O_WDRW | O_NONBLOCK
            -   fcntl
                -   int flags = fcntl(fd,F_GETFL);
                -   flag |= O_NONBLOCK
                -   fcntl(fd,F_SETFL,flag);
        -   将缓冲区的全部数据都读出


## TCP、UDP使用场景

TCP使用场景

-   对数据安全性要求高的时候

-   登录数据的传输
-   文件传输
-   http协议

UDP使用场景

-   效率高，实时性要求比较高
-   视频聊天
-   通话



## 心跳包

1.  判断客户端和服务器是否处于连接状态
    -   心跳机制
        -   不会携带大量数据
        -   每隔一定时间服务器->客户端/客户端->服务器发送一个数据包
    -   心跳包看成一个协议
        -   应用层协议
    -   判断网络是否断开
        -   有多个连续的心跳包没收到/没有回复
        -   关闭通信的套接字
    -   重连
        -   重新开始套接字
        -   继续发送心跳包



