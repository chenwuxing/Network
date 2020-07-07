#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/listener.h>
#include<arpa/inet.h>

// 读回调
void read_cb(struct bufferevent *bev,void *arg)
{
    // 接收对方发过来的数据
    char buf[1024] = {0};
    int len = bufferevent_read(bev,buf,sizeof(buf));
    printf("recv data:%s\n",buf);

    // 给对方发数据
    bufferevent_write(bev,buf,len+1);
}

// 写回调
void write_cb(struct bufferevent *bev,void *arg)
{
    printf("我是一个没有卵用的函数\n");
}

// 事件回调
void event_cb(struct bufferevent *bev,short events,void *arg)
{
    if(events & BEV_EVENT_EOF)
    {
        printf("connection closed\n");
    }
    else if(events & BEV_EVENT_ERROR)
    {
        printf("some other error\n");
    }
    else if(events & BEV_EVENT_CONNECTED)
    {
        printf("已经成功连接服务器...\n");
        return;
    }
    // 释放资源
    bufferevent_free(bev);

}

// 终端接收输入
void read_terminal(int fd,short what,void *arg)
{
    // 读终端里的数据
    char buf[1024] ={0};
    int len = read(fd,buf,sizeof(buf));
    // 将数据发送给server
    struct bufferevent *bev = (struct bufferevent*)arg;
    bufferevent_write(bev,buf,len+1);


}
int main()
{
    // 创建事件处理框架
    struct event_base *base = event_base_new();

    //init 服务器
    struct sockaddr_in serv;
    memset(&serv,0,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8765);
    inet_pton(AF_INET,"127.0.0.1",&serv.sin_addr.s_addr);

    // 创建事件，事件其实就是对套接字的封装
    int fd = socket(AF_INET,SOCK_STREAM,0);
    struct bufferevent *bev = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    // 连接服务器
    bufferevent_socket_connect(bev,(struct sockaddr*)&serv,sizeof(serv));

    // 给缓冲区设置回调
    bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
    bufferevent_enable(bev,EV_READ);

    // 接收键盘输入
    // 创建一个新事件
    struct event *ev = event_new(base,STDIN_FILENO,EV_READ | EV_PERSIST,read_terminal,bev);

    // 添加事件到event_base
    event_add(ev,NULL);

    // 启动事件循环
    event_base_dispatch(base);
    //释放资源
    event_base_free(base);

}