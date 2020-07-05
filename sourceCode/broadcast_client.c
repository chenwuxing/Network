#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc,char *argv[])
{
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd == -1)
    {
        perror("socket error");
        exit(1);
    }

    // 客户端需要显示的绑定ip和端口
    struct sockaddr_in client;
    memset(&client,0,sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(6767);
    inet_pton(AF_INET,"0.0.0.0",&client.sin_addr.s_addr);

    // 绑定
    int ret = bind (fd,(struct sockaddr*)&client,sizeof(client));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    // 通信-接收数据
    while(1)
    {
        char buf[1024] = {0};
        int len = recvfrom(fd,buf,sizeof(buf),0,NULL,NULL);
        if(len == -1)
        {
            perror("recvfrom error");
            break;
        }
        printf("client == recv buf: %s\n",buf);
    }

    close(fd);
    return 0;
}