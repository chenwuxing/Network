#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc,char *argv[])
{
    // 创建套接字
    int fd = socket(AF_INET,SOCK_DGRAM,0);

    // 绑定本地ip和端口
    struct sockaddr_in serv;
    memset(&serv,0,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8765);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    // 监听
    int ret = bind(fd,(struct sockaddr*)&serv,sizeof(serv));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }
    struct sockaddr_in client;
    socklen_t cli_len = sizeof(client);
    // 通信
    char buf[1024] = {0};
    while(1)
    {
        int recvlen = recvfrom(fd,buf,sizeof(buf),0,(struct sockaddr*)&client,&cli_len);

        if(recvlen == -1)
        {
            perror("recvform error");
            exit(1);
        }
        printf("recv buf:%s\n",buf);
        char ip[64];
        printf("New client IP:%s,Port:%d\n",
            inet_ntop(AF_INET,&client.sin_addr.s_addr,ip,sizeof(ip)),
            ntohs(client.sin_port));
        // 给客户端发送数据
        sendto(fd,buf,strlen(buf)+1,0,(struct sockaddr*)&client,sizeof(client));
    }
    close(fd);
    return 0;


}