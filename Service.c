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

/*
 * List为自定义的头文件，里面包含了对单向链表的操作：
 * List_Init(head,list_node_t)  单向链表初始化宏定义
 * List_AddHead(head,newNode)   单项链表头插发宏定义
*/
#include"List.h"

/*
 *  Persits.h 封装了部分文件操作函数，具体如下：
 *  int Register_Persist(message_node_t *buf)  //用户信息写入函数，返回1 表示操作成功，0 表示操作失败
 *  int UserInfo_Perst_Select(char *name,message_node_t *buf)  //通过用户名，找到用户相关信息返回1表示找到，0表示未找到
 *  int Play_Perst_Update(const message_node_t *data)  ////将参数所指向的新信息写入到文件中，返回0表示操作失败，返回1表示操作成功
 */

#include"Persist.h"
//宏定义

#define SERV_PORT 8080      //端口
#define MAX_LIST 10         //最长等待序列
#define BUFSIZE 1024        //缓冲区大小
//交互信息结构体

/*
typedef struct Message
{
    int flag;               //0 系统命令，1 用户注册，2 用户登录，3 用户群聊,4 用户私聊
    char Sendname[21];      // 发送者，系统为system 
    char Recvname[21];      // 接收者，系统为system ，所有人为everyone 
    char Message[BUFSIZE];      // 消息内容
    time_t Sendtime;        // 发送时间
    struct Message *head;
    struct Message *prev;
}
*/

/*
 *  在线用户链表  
*/

typedef struct On_Line     
{
    char name[21];    //在线的用户名
    int sock_fd;      //在线用户套接字描述符
    struct On_Line *next;
    struct On_Line *prev;
} online_node_t;

//在线用户数量

int fd_count=0;

//在线用户链表头结点（全局变量）

online_node_t *head;

int Info_Match(char *name,char *passwd)  //信息匹配函数，用于进行密码和用户名的验证,0表示操作失败，1表示操作成功
{
    int rtn=0;
    message_node_t buf;
    if(UserInfo_Perst_Select(name,&buf))
    {
        printf("name: %s passwd:%s\n",buf.Sendname,buf.Recvname);
        printf("Name: %s Passwd:%s\n",name,passwd);
        if(!strcmp(buf.Recvname,passwd))
        {
            rtn=1;
        }
    }
    return rtn;
}

int Log_Service(int conn_fd,char *newName) //登录/注册信息服务函数
{
    int rtn=0;
    int i;
    message_node_t recv_buf,send_buf;
    time_t now;
    printf("Test conn_fd=%d\n",conn_fd);
    if(recv(conn_fd,&recv_buf,sizeof(message_node_t),0)<0)
    {
        perror("recv");
        exit(0);
    }
    switch(recv_buf.flag)
    {
        case 1:
            for(i=0;;i++)
            {
                int sign=0;
                printf("Service Test: %d ",i);
                if(UserInfo_SelectByName(recv_buf.Sendname))  //对用户名存在性进行检测，如果存在，则直接返回
                {  
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"Your Nickname Already Exists!");
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        perror("send");
                        exit(0);
                    }
                    sign=1;
                }
                else
                {
                    if(Register_Persist(&recv_buf))
                    {
                        send_buf.flag=0;
                        strcpy(send_buf.Sendname,"system");
                        strcpy(send_buf.Recvname,recv_buf.Sendname);
                        time(&now);
                        send_buf.Sendtime=now;
                        strcpy(send_buf.Message,"Success");
                        strcpy(newName,recv_buf.Sendname);
                        if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                        {
                            perror("send");
                            exit(0);
                        }
                        break;
                    }
                    else
                    {      
                        send_buf.flag=0;
                        strcpy(send_buf.Sendname,"system");
                        strcpy(send_buf.Recvname,recv_buf.Sendname);
                        time(&now);
                        send_buf.Sendtime=now;
                        strcpy(send_buf.Message,"Writing To File Fail !");
                        if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                        {
                            perror("send");
                            exit(0);
                        }
                    }
                    sign=1;
                }

                if(sign)
                {
                    memset(&recv_buf,0,sizeof(message_node_t));
                    if(recv(conn_fd,&recv_buf,sizeof(message_node_t),0)<0)
                    {
                        perror("recv");
                        exit(0);
                    }
                }
            }
            close(conn_fd);
            break;
        case 2:
            for(i=0;i<3;i++)
            {
                printf("Service Test %d\n",i);
                memset(&send_buf,0,sizeof(message_node_t));
                int sign=0;
                if(Info_Match(recv_buf.Sendname,recv_buf.Recvname))
                {
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"Success");
                    printf("Sign in success \n");
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        perror("send");
                        exit(0);
                    }
                    strcpy(newName,recv_buf.Sendname);
                    rtn=1;
                    break;
                }
                else
                {
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"ERROR Incorrect Username Or Password!");
                    printf("%s\n",send_buf.Message);
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        perror("send");
                        exit(0);
                    }
                    sign=1;
                }
                memset(&recv_buf,0,sizeof(message_node_t));
                if(sign)
                { 
                    if(recv(conn_fd,&recv_buf,sizeof(message_node_t),0)<0)
                    {
                        perror("recv");
                        exit(0);
                    }   
                }
            }
    }
    return rtn;
}
void Send_Message(message_node_t *buf)
{
    int j;
    online_node_t *t;
    t=head->next;
    for(j=0;j<fd_count;j++)
    {
        if (t->sock_fd==head->prev->sock_fd || !strcmp(t->name,buf->Sendname))
            continue;
        if(send(t->sock_fd,buf,sizeof(online_node_t),0)<0)
        {
            perror("send");
        }
        t=t->next;
    }
    
    /*
    switch(buf->flag)
    {
        case 3:
            for(j=0;j<fd_count;j++)
            {
                if (t->sock_fd==head->prev->sock_fd)
                    continue;
                if(send(t->sock_fd,buf,sizeof(online_node_t),0)<0)
                {
                    perror("send");
                }
                t=t->next;
            }
            break;
        case 4:
            for(j=0;j<fd_count;j++)
            {
                if (t->sock_fd==head->prev->sock_fd||strcmp(t->name,buf->Recvname))
                    continue;
                if(send(t->sock_fd,buf,sizeof(online_node_t),0)<0)
                {
                    perror("send");
                }
                t=t->next;
            }
            break;
    }
    */
}
int main()
{
    int sock_fd;
    int conn_fd;
    int optval;
    int ret;
    int srv_len,clt_len;
    int flag_recv=0;
    pid_t pid,vpid;
    struct sockaddr_in srv_sock,clt_sock;
    message_node_t recv_buf;
    online_node_t *p;
    List_Init(head,online_node_t);
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
    strcpy(p->name,"system");
    p->sock_fd=sock_fd;
    List_AddHead(head,p);
    fd_count++;
    srv_len=sizeof(struct sockaddr_in);
    FD_ZERO(&readfds); 
    FD_SET(sock_fd,&readfds);
    while(1)
    {
        int fd;
        int nread;
        int j;
        int flag=0;
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
            printf(" %d name:%s  sock_fd:%d\n",fd,s->name,s->sock_fd);
            if(FD_ISSET(s->sock_fd,&testfds))  //检测出现响应的在不在在线用户链表中，不在责不进行操作，直接跳过
            {
                if(s->sock_fd==sock_fd) //如果响应的是监听套接字，则说明是一个新的用户请求
                {
                    clt_len=sizeof(struct sockaddr_in);
                    conn_fd=accept(s->sock_fd,(struct sockaddr *)&clt_sock,&clt_len);
                    
                    //使用vfork创建一个进程，用于进行密码注册用户或登录验证
                    
                    if(vpid=vfork()==0)
                    {
                        int sign=0;
                        char newName[21];
                        sign=Log_Service(conn_fd,newName);
                        
                        if(sign==1)
                        {
                            p=(online_node_t *)malloc(sizeof(online_node_t));
                            strcpy(p->name,newName);
                            p->sock_fd=conn_fd;
                            List_AddHead(head,p);                     
                            FD_SET(p->sock_fd,&readfds);
                            fd_count++;
                            printf("adding client on fd %d name:%s\n",p->sock_fd,p->name);
                        }
                    }
                }
                else  //否则，表示是已在线的用户出现操作
                {
                    memset(&recv_buf,0,sizeof(message_node_t));  //每次先将缓冲区清空
                    nread = recv(s->sock_fd,&recv_buf,sizeof(message_node_t),0); //接受用户发送来的数据
                    if(nread==0) //如果数据长度为零，则表示用户离开
                    {
                        online_node_t *r;
                        r=s;
                        s=s->next;
                        flag=1;
                        FD_CLR(r->sock_fd,&readfds);
                        close(r->sock_fd);                
                        printf("removeing clinet on fd %d\n",fd);
                        List_FreeNode(r);
                        fd_count--;
                    }
                    else 
                    {
                        if((pid=fork())==0)  //创建一个新的进程，用于处理用户发来的信息，解析并转发给其他用户
                        {
                            Send_Message(&recv_buf);
                            exit(0);
                        }
                        //printf("recv = %s",recv_buf);
                    }
                }
            }
        }
    }
}
