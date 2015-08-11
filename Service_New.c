/*************************************************************************
	> File Name: Service.c
	> Author: ZhouPan / github:dreamer2018 
	> Blog: blog.csdn.net/it_dream_er 
	> Mail: zhoupans_mail@163.com 
	> Created Time: Wed 05 Aug 2015 10:21:26 AM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<error.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<time.h>
#include"conio.h"

#define SERV_PORT 8000
#define MAX_LIST 10
#define USERNAME 0
#define PASSWORD 1
#define BUFSIZE 1024



//#include"Persist.h"
#include"List.h"

typedef struct On_Line     
{
    char name[21];    //在线的用户名
    int sock_fd;      //在线用户套接字描述符
    struct On_Line *next;
    struct On_Line *prev;
} online_node_t;

online_node_t *head;

int fd_count=0;

int main()
{
    int sock_fd;
    int conn_fd;
    int optval;
    int srv_len,clt_len;
    int ret;
    int flag=0;
    int flag_recv=0;
    int switch_num;
    List_Init(head,online_node_t);
    online_node_t *p;
    pid_t pid;
    struct sockaddr_in srv_sock,clt_sock;
    char recv_buf[BUFSIZE];
    fd_set readfds,testfds;
    
     //创建一个TCP 套接字
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd<0)
    {
        perror("socket");
    }

    //设置该套接字使之可以重新绑定
    optval=1;
    
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int))<0)
    {
        perror("setsockopt");
    }
    
    //初始化本地端
    memset(&srv_sock,0,sizeof(struct sockaddr_in ));
    
    srv_sock.sin_family=AF_INET;
    srv_sock.sin_port=htons(SERV_PORT);
    srv_sock.sin_addr.s_addr=htonl(INADDR_ANY);
    
    //绑定套接字到本地端
    if(bind(sock_fd,(struct sockaddr *)&srv_sock,sizeof(struct sockaddr_in ))<0)
    {
        perror("bind");
    }
    
    //设置监听
    if(listen(sock_fd,MAX_LIST)<0)
    {
        perror("listen");
    }
    
    p=(online_node_t *)malloc(sizeof(online_node_t));
    p->sock_fd=sock_fd;
    srv_len=sizeof(struct sockaddr_in);
    List_AddHead(head,p);
    fd_count++;
    FD_ZERO(&readfds);
    FD_SET(sock_fd,&readfds);
    while(1)
    {
        int fd;
        int nread;
        int j;
        online_node_t *s;
        s=head;
        testfds=readfds;
        
        printf("service waiting\n");
        ret=select(MAX_LIST, &testfds ,(fd_set *)0,(fd_set *)0,(struct timeval *)0);
        if(ret<0)
        {
            perror("select");
        }
        
        for(fd=0;fd<fd_count;fd++)
        {
            if(flag!=1)
                s=s->next;
            flag=0;
            if(FD_ISSET(s->sock_fd,&testfds))
            {
                if(s->sock_fd==sock_fd)
                {
                    clt_len=sizeof(struct sockaddr_in);
                    conn_fd=accept(sock_fd,(struct sockaddr *)&clt_sock,&clt_len);
                    FD_SET(conn_fd,&readfds);
                    p=(online_node_t*)malloc(sizeof(online_node_t));
                    p->sock_fd=conn_fd;
                    List_AddHead(head,p);
                    fd_count++;
                    printf("adding client on fd %d\n",p->sock_fd);
                }
                else
                {
                    memset(recv_buf,0,sizeof(recv_buf));
                    nread = recv(s->sock_fd,recv_buf,sizeof(recv_buf),0);
                    if(nread==0)
                    {
                        online_node_t *h;
                        h=s;
                        s=s->next;
                        flag=1;
                        FD_CLR(h->sock_fd,&readfds);
                        close(h->sock_fd);
                        printf("removeing clinet on fd %d\n",h->sock_fd);
                        List_FreeNode(h);                  
                        fd_count--;
                    }
                    else
                    {
                        int n;
                        n=s->sock_fd;
                        if((pid=fork())==0)
                        {
                            online_node_t *r;
                            r=head->next;
                            printf("Tets fd_count=%d\n",fd_count);
                            for(j=0;j<fd_count;j++)
                            {
                                if (r->sock_fd == head->prev->sock_fd || r->sock_fd==n)
                                    continue;
                                if(send(r->sock_fd,recv_buf,sizeof(recv_buf),0)<0)
                                {
                                    perror("send");
                                }
                                r=r->next;
                            }
                            exit(0);
                        }
                        //printf("recv = %s",recv_buf);
                    }
                }
            }
        }
    }
}
