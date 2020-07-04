#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<errno.h>
#include<pthread.h>

// 自定义数据结构
typedef struct SockInfo
{
    int fd;
    struct sockaddr_in addr;
    pthread_t id;
}SockInfo;

// 子线程处理函数
void* worker(void *arg)
{
    char ip[64];
    char buf[1024];
    SockInfo *info = (SockInfo*)arg;
    // 通信
    while(1)
    {
        printf("client IP:%s,port:%d\n",
            inet_ntop(AF_INET,&info->addr.sin_addr.s_addr,ip,sizeof(ip)),
            ntohs(info->addr.sin_port));
        int len = read(info->fd,buf,sizeof(buf));
        if(len == -1)
        {
            perror("read error");
            pthread_exit(NULL);
        }
        else if(len == 0)
        {
            printf("客户端已经断开了连接\n");
            close(info->fd);
            break;
        }
        else
        {
            printf("recv buf:%s\n",buf);
            write(info->fd,buf,len);
        }
        
    }
    return NULL;
}

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        printf("eg:./a.out port\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(serv_addr);
    int port = atoi(argv[1]);

    // 创建套接字
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    // 初始化服务器 sockaddr_in
    memset(&serv_addr,0,serv_len);
    serv_addr.sin_family = AF_INET; // 地址族
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本机所有IP
    serv_addr.sin_port = htons(port); // 设置端口
    // 绑定IP和端口
    bind(lfd,(struct sockaddr*)&serv_addr,serv_len);

    // 设置同时监听的最大个数
    listen(lfd,36);
    printf("start accept .....\n");

    unsigned int i = 0;
    SockInfo info[256];
    // 规定未被使用的文件描述符的位置id值为-1
    for(int i = 0;i < sizeof(info) / sizeof(info[0]);i++)
    {
        info[i].fd = -1;
    }
    socklen_t cli_len = sizeof(struct sockaddr_in);
    while(1)
    {
        // 选一个未被使用的
        for(i = 0;i < 256;i++)
        {
            if(info[i].fd == -1)
                break;
        }
        if(i == 256)
        {
            break;
        }
        // 主线程-等待接收连接请求
        info[i].fd = accept(lfd,(struct sockaddr*)&info[i].addr,&cli_len);
        // 创建子线程-通信
        pthread_create(&info[i].id,NULL,worker,&info[i]);
        //设置线程分离
        pthread_detach(info[i].id);

    }
    close(lfd);
    // 只退出主线程
    pthread_exit(NULL);
    return 0;
}
