#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<event2/listener.h>

// 读回调
void read_cb(struct bufferevent *bev,void *arg)
{
    // 读缓冲区中的数据 
    char buf[1024] = {0};
    bufferevent_read(bev,buf,sizeof(buf));
    printf("recv buf:%s\n",buf);

    char *pt = "你发送的数据我已经收到了....";
    // 发送数据   往缓冲区里写数据
    bufferevent_write(bev,pt,strlen(pt)+1);
    printf("我发送了数据，给客户端......\n");
    
}

// 写回调
void write_cb(struct bufferevent *bev,void *arg)
{
    printf("发送的数据已经发送出去了....\n");
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
    // 释放bufferevent资源
    bufferevent_free(bev);
    
}

// 连接完成之后执行的回调
void listen_cb(struct evconnlistener *listener,
                evutil_socket_t fd,struct sockaddr *addr,int len,void *ptr)
{
    // 得到传进来的event_base
    struct event_base *base = (struct event_base*)ptr;
    // 先接收数据 --发送数据
    // 将fd封装为bufferevent

    struct bufferevent *bev = NULL;
    bev = bufferevent_socket_new(base,fd,BEV_OPT_CLOSE_ON_FREE);
    // 给bufferevent对应的读写缓冲区设置回调函数
    bufferevent_setcb(bev,read_cb,write_cb,event_cb,NULL);
    //设置读缓冲区的回调可用，默认写是可用的
    bufferevent_enable(bev,EV_READ);

}

int main(int argc,char *argv[])
{
    // 创建事件处理框架
    struct event_base* base = event_base_new();
    // 创建监听的套接字
    // 绑定
    //监听
    //等待并接收连接请求
    // 服务器ip、端口地址
    struct sockaddr_in serv;
    memset(&serv,0,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(9888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    struct evconnlistener* listen = NULL;
    listen = evconnlistener_new_bind(base,listen_cb,NULL,
        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,-1,(struct sockaddr*)&serv,sizeof(serv));
    
    // 进入到事件循环
    event_base_free(base);

    // 释放资源
    evconnlistener_free(listen);
    event_base_free(base);
}