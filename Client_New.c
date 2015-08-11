/*************************************************************************
	> File Name: Client.c
	> Author: ZhouPan / github:dreamer2018 
	> Blog: blog.csdn.net/it_dream_er 
	> Mail: zhoupans_mail@163.com 
	> Created Time: Wed 05 Aug 2015 04:03:15 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define INVALID_USERINFO 'n'    //用户信息无效
#define VALID_USERINFO 'y'      //用户信息有效
#define BUFMAX 1024

void *threadrecv(void *vargp);
void *threadsend(void *vargp);

int my_recv(int conn_fd,char *data_buf,int len)  //
{
    static char recv_buf[BUFMAX];
    static char *pread;
    static int len_remain=0;
    int i;
    
    if(len_remain<=0)
    {
        if((len_remain=recv(conn_fd,recv_buf,sizeof(recv_buf),0))<0)
        {
            perror("recv");
        }
        else if(len_remain==0) //目的计算机的socket连接关闭
        {
            return 0;
        }
        pread=recv_buf; //重新初始化pread
    }
    
    for(i=0;*pread!='\n';i++)
    {
        if(i>len)
        {
            return -1;
        }
        data_buf[i]=*pread++;
        len_remain--;
    }
    //去除结束标志
    len_remain--;
    pread++;

    return i;    //读取成功
}

int main(int argc,char *argv[])
{   
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    int *clientfdp;
    int status;
    clientfdp=(int *)malloc(sizeof(int));
    struct sockaddr_in serv_addr;
    char recv_buf[BUFMAX];

    if(argc!=5)
    {
        printf("Usage: [-p] [serv_port] [-a] [serv_address] \n");
        exit(1);
    }
    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family=AF_INET;
    //从命令行获取服务器端的端口和地址
    for(i=0;i<argc;i++)
    {
        if(strcmp("-p",argv[i])==0)
        {
            serv_port=atoi(argv[i+1]);  //将字符串转换成整型
            if(serv_port<0||serv_port>65535)
            {
                printf("invalid serv_addr.sin_port\n");
                exit(1);
            }
            else
            {
                serv_addr.sin_port=htons(serv_port);
            }
            continue;
        }
        
        if(strcmp("-a",argv[i])==0)
        {
            if(inet_aton(argv[i+1],&serv_addr.sin_addr)==0)
            {
                printf("invalid server ip address\n");
                exit(1);
            }
            continue;
        }
    }

    //检测数否少输入了某项参数
    if(serv_addr.sin_port==0||serv_addr.sin_addr.s_addr==0)
    {
        printf("Usage :[-p] [serv_addr.sin_port] [-a] [serv_address]\n");
        exit(1);
    }
    conn_fd=socket(AF_INET,SOCK_STREAM,0);
    if(conn_fd<0)
    {
        perror("socket");
    }
    
    if(connect(conn_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))<0)
    {
        perror("connect");
        exit(1);
    }
	pthread_t tid1,tid2;
    printf("connected\n");
    
    pthread_create(&tid1,NULL,threadsend,&conn_fd);
    pthread_create(&tid2,NULL,threadrecv,&conn_fd);
    pthread_join(tid1,(void *)&status);
    pthread_join(tid2,(void *)&status);
    close(conn_fd);
    return 0;
}

void *threadsend(void * vargp)
{
    //pthread_t tid2;
    int connfd = *((int *)vargp);
    
    int idata;
    char temp[BUFMAX];
    while(1)
    {
        fgets(temp,BUFMAX,stdin);
        send(connfd,temp,BUFMAX,0);
        printf("    client send OK\n");
    }


    printf("client send\n");
    return NULL;
}


void *threadrecv(void *vargp)
{
    char temp[BUFMAX];
    int connfd = *((int *)vargp);   
    while(1)
    {
        int idata = 0;
        idata = recv(connfd,temp,BUFMAX,0);
        if(idata > 0)
        {
            printf("server :%s",temp);
        }
        //printf("Test\n");
    }
    return NULL;
}
