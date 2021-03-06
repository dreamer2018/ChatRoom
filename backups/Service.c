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
#include<errno.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<time.h>

#include"List.h"

/*
 * List为自定义的头文件，里面包含了对单向链表的操作：
 * List_Init(head,list_node_t)  单向链表初始化宏定义
 * List_AddHead(head,newNode)   单项链表头插发宏定义
*/
#include"my_error.h"

/*
 *  my_error.h 封装了所有的系统日志文件，包括：
 *  struct System_Error_log  系统错误记录结构体
 *  struct System_Register_log 系统登录注册记录结构体
 *  void Error_Log(char *name,char *message)    错误处理函数，name为出错时的用户名，message为出错信息
 *  void Register_Log(int type,char *name,char *addr,char *message)  //type为记录的类型,0为注册，1为登录
 *  void Register_Persist_Log(error_node_t *buf)     登录/注册信息文件写入函数
 *  void Error_Persist_Log(register_node_t *buf)     错误信息写入函数
 */


#include"Persist.h"

/*
 *  Persits.h 封装了部分文件操作函数，具体如下：
 *  struct date   //日期保存结构体
 *  int DateCmp(data_t dt1,data_t dt2)  //日期比较函数 相等返回0，dt1<dt2 ,返回-1,否则，返回1
 *  int DateCmp_Srv(data_t dt1,data_t dt2,time_t time) //时间范围比较函数，在范围内返回1不在返回0
 *  int Exists(char *path) //目录存在与否检测函数，在返回1，不在返回0
 *  int System_Init()  //系统初始化函数，创建系统必需的文件夹
 *  int User_Init(char *user_name)  //用户初始化函数，创建用户自己的文件夹
 *  int Client_Init(char *user_name) //客户端初始化函数，创建客户端所必需的文件夹
 *  int Register_Persist(message_node_t *data) //用户信息写入函数，成功1，失败0
 *  int Service_Group_Message_Save(char *file,message_node_t *data)  //用户群消息写入函数,返回1 表示操作成功，0 表示操作失败
 *  int Service_Message_Save(char *file,message_node_t *data) //用户消息写入函数，1成功，0失败
 *  int Offline_Message_Save(message_node_t *data) //离线消息保存函数
 *  int Offline_Message_Select(char *Recvname,message_node_t *head)  //通过用户名，找到用户相关信息返回1表示找到，0表示未找到
 *  int Client_Message_Save(char *name,message_node_t *data) //客户端私聊信息保存函数，0失败，1成功
 *  int Client_Group_Message_Save(char *name,message_node_t *data)  //用户信息写入函数,返回1 表示操作成功，0 表示操作失败
 *  int UserInfo_SelectByName(char *name) //通过用户名查找该用户是否存在,1表示存在。0表示不存在
 *  int UserInfo_Perst_Select(char *name,message_node_t *buf)  //通过用户名，找到用户相关信息返回1表示找到，0表示未找到
 *  int User_Passwd_Update(message_node_t *data)  //将参数所指向的新信息写入到文件中，返回0表示操作失败，返回1表示操作成功
 *  int Private_Message_SelectByTime(char *name,date_t dt1,date_t dt2,message_node_t *list)  //通过时间范围查询私聊信息
 *  int Group_Message_SelectByTime(char *name,date_t dt1,date_t dt2,message_node_t *list) //通过时间范围查询群聊信息
 *  int Private_Message_SelectByName(char *name,char *select_name,message_node_t *list)  //通过用户名查询用户私聊信息
 *  int Group_Message_SelectByName(char *name,char *select_name,message_node_t *list)  //通过用户名查询用户群聊信息
 *  int Private_Message_SelectAll(char *name,message_node_t *list)  //获取所有私聊信息函数
 *  int Group_Message_SelectAll(char *name,message_node_t *list)  //获取所有私聊信息函数
 *  int Group_Message_SelectByTime(char *name,date_t dt1,date_t dt2,message_node_t *list) //通过时间范围查询群聊信息 
 *  int Private_Message_SelectByName(char *name,char *select_name,message_node_t *list)  //通过用户名查询用户私聊信息
 *  int Group_Message_SelectByName(char *name,char *select_name,message_node_t *list)  //通过用户名查询用户群聊信息
 *  int Private_Message_SelectAll(char *name,message_node_t *list)  //获取所有私聊信息函数
 *  int Group_Message_SelectAll(char *name,message_node_t *list)  //获取所有群聊信息函数
 */


//宏定义

#define SERV_PORT 8080      //端口
#define MAX_LIST 20         //最长等待序列

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
    char name[21];          //在线的用户名
    int sock_fd;            //在线用户套接字描述符
    char address[20];       //在线用户ip
    struct On_Line *next;   
    struct On_Line *prev;
} online_node_t;

//在线用户数量

int fd_count=0;
extern int errno;


online_node_t *head; //在线用户链表头结点（全局变量）

int Info_Match(char *name,char *passwd)  //信息匹配函数，用于进行密码和用户名的验证,0表示操作失败，1表示操作成功
{
    int rtn=0;
    message_node_t buf;
    if(UserInfo_Perst_Select(name,&buf))
    {
        if(!strcmp(buf.Recvname,passwd))
        {
            rtn=1;
        }
    }
    return rtn;
}

int OnLine_Find_ByName(char *name) //在线用户查询函数，在返回1不在返回0
{
    int i,rtn=0;
    online_node_t *p;
    p=head;
    for(i=0;i<fd_count;i++)
    {
        p=p->next;
        if(!strcmp(p->name,name))
        {
            return 1;
        }
    }
    return 0;
}

int Offline_Message_Send(int conn_fd,char *name)  //离线消息发送函数,成功发送返回1，失败返回0
{
    int count=0,i;
    message_node_t *head,*p;
    List_Init(head,message_node_t);
    count=Offline_Message_Select(name,head);
    printf("count=%d\n",count);
    p=head;
    if(count>0)
    {
        for(i=0;i<count;i++)
        {
            p=p->next;
            if(send(conn_fd,p,sizeof(message_node_t),0)<0)
            {
                Error_Log("send: ",strerror(errno));
                return 0;
            }
        }
    }
    return 1;
}

int Log_Service(int conn_fd,char *newName,char *address) //登录/注册信息服务函数,成功返回1，失败返回0
{
    int rtn=0;
    message_node_t recv_buf,send_buf;
    time_t now;
    if(recv(conn_fd,&recv_buf,sizeof(message_node_t),0)<0) //接受用户发送来的数据
    {
        Error_Log("recv: ",strerror(errno));
        exit(0);
    }
    switch(recv_buf.flag) //通过flag对数据内容进行解析
    {
        case 1:  //用户注册数据包
            if(UserInfo_SelectByName(recv_buf.Sendname))  //对用户名存在性进行检测，如果存在，则直接发送昵称重复函数
            {  
                send_buf.flag=0;
                strcpy(send_buf.Sendname,"system");
                strcpy(send_buf.Recvname,recv_buf.Sendname);
                time(&now);
                send_buf.Sendtime=now;
                strcpy(send_buf.Message,"Your Nickname Already Exists! Please Try Again....");    
                Register_Log(0,recv_buf.Sendname,address,"Nickname Already Exists");
                if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send: ",strerror(errno));
                    exit(0);
                }
            }
            else //否则说明昵称正确，准备保存
            {
                if(Register_Persist( &recv_buf )) //写入成功，则返回注册成功
                {
                    
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"Success");
                    strcpy(newName,recv_buf.Sendname);
                    Register_Log(0,recv_buf.Sendname,address,"register success");
                    User_Init(newName);
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        Error_Log("send: ",strerror(errno));
                        exit(0);
                    }
                }
                else //写入失败，则返回注册失败
                {      
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"Writing To File Fail !");
                    Error_Log("Register_Persist: ","Writing To File Fail");
                    Register_Log(0,recv_buf.Sendname,address,"register fail,Writing To File Fail");
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        Error_Log("send: ",strerror(errno));
                        exit(0);
                    }
                }
            }
            close(conn_fd);
            break;
        case 2:  //用户登录数据包
            if(Info_Match(recv_buf.Sendname,recv_buf.Recvname)) //检查用户名，密码是否匹配
            {
                if(OnLine_Find_ByName(recv_buf.Sendname)) //判断用户是否在线,在线的话就提醒重复登录
                {
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"This User Has Logged In ,You Can Not Log In Again ");
                    Register_Log(1,recv_buf.Sendname,address,"login fail,this name has logged in");             
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        Error_Log("send: ",strerror(errno));
                        exit(0);
                    }
                    rtn=0;
                }
                else //不在线则提醒登录成功
                {  
                    send_buf.flag=0;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,recv_buf.Sendname);
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Message,"Success");
                    Register_Log(1,recv_buf.Sendname,address,"login success");             
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        Error_Log("send: ",strerror(errno));
                        exit(0);
                    }
                    strcpy(newName,recv_buf.Sendname);
                    rtn=1;
                }
            }
            else //密码错误或用户名不存在则提醒用户名或密码错误错误
            {
                send_buf.flag=0;
                strcpy(send_buf.Sendname,"system");
                strcpy(send_buf.Recvname,recv_buf.Sendname);
                time(&now);
                send_buf.Sendtime=now;
                strcpy(send_buf.Message,"ERROR Incorrect Username Or Password!");
                Register_Log(1,recv_buf.Sendname,address,"ERROR Incorrect Username Or Password");
                if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send: ",strerror(errno));
                    exit(0);
                }
            }
    }
    return rtn;
}

void System_command(message_node_t *buf,int conn_fd) //系统命令解析函数
{
    int i;
    online_node_t *p;
    message_node_t send_buf;
    p=head;
    time_t now;
    if(!strncmp(buf->Message,"quit",4)) //客户端退出命令
    {
        for(i=0;i<fd_count;i++)
        {
            p=p->next;
            if (!strcmp(p->name,buf->Sendname))
            {
                time(&now);
                buf->Sendtime=now;
                if(send(p->sock_fd,buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send: ",strerror(errno));
                    exit(0);
                }
            }
        }
    }
    else if(!strncmp(buf->Message,"online",6)) //查询在线用户命令
    {
        time(&now);
        send_buf.flag=6;
        send_buf.Sendtime=now;
        strcpy(send_buf.Sendname,"system");
        strcpy(send_buf.Recvname,buf->Sendname);
        sprintf(send_buf.Message,"Total %d People Online,Threre is:",fd_count-1);
        if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
        {
            Error_Log("send: ",strerror(errno));
            exit(0);
        }
        for(i=0;i<fd_count-1;i++)
        {
            p=p->next;
            memset(&send_buf,0,sizeof(message_node_t));
            time(&now);
            send_buf.flag=6;
            send_buf.Sendtime=now;
            strcpy(send_buf.Sendname,"system");
            strcpy(send_buf.Recvname,buf->Sendname);
            strcpy(send_buf.Message,p->name);
            if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
            {
                Error_Log("send: ",strerror(errno));
                exit(0);
            } 
        }
    }
    else //如果没有输入的命令,则提示命令不存在
    {
        time(&now);
        send_buf.flag=6;
        send_buf.Sendtime=now;
        strcpy(send_buf.Sendname,"system");
        strcpy(send_buf.Recvname,buf->Sendname);
        sprintf(send_buf.Message,"Not Found This Commond: %s",buf->Message);
        
        if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
        {
            Error_Log("send: ",strerror(errno));
            exit(0);
        }
        
    }
}

int Change_Password_Srv(message_node_t *buf) //更改登录密码服务函数
{
    time_t now;
    message_node_t data;
    UserInfo_Perst_Select(buf->Sendname,&data);
    if(!strcmp(buf->Message,data.Recvname)) //检测旧密码是否匹配
    {
        User_Passwd_Update(buf); //更新密码
        return 1;
    }
    else
    {
        return 0;
    }
}
int Send_Message(int conn_fd,message_node_t *buf) //数据包解析转发函数
{
    int j;
    message_node_t send_buf;
    online_node_t *t;
    t=head;
    time_t now;
    switch(buf->flag) //对数据包进行解析函数
    {
        case 0: //系统命令
            System_command(buf,conn_fd); 
            break;
        case 3: //用户群聊
            Service_Group_Message_Save("./user/group/Chat.dat",buf); //保存群聊信息函数
            for(j=0;j<fd_count;j++)
            {          
                t=t->next;
                if (t->sock_fd==head->prev->sock_fd || !strcmp(t->name,buf->Sendname))
                    continue;
                if(send(t->sock_fd,buf,sizeof(message_node_t),0)<0) 
                {
                    Error_Log("send: ",strerror(errno));
                    exit(0);
                }
            }
            break;
        case 4: //用户私聊
            if(UserInfo_SelectByName(buf->Recvname)) //检验要私聊的用户是否注册
            {
                Service_Message_Save(buf->Sendname,"./user/",buf);
                Service_Message_Save(buf->Recvname,"./user/",buf);
                if(OnLine_Find_ByName(buf->Recvname)) //如果在线的话，消息会直接发送到接收者的客户端上，在此就不需要处理
                {
                    for(j=0;j<fd_count;j++)
                    {
                        t=t->next;
                        if (!strcmp(t->name,buf->Recvname))
                        {
                            if(send(t->sock_fd,buf,sizeof(message_node_t),0)<0)
                            {
                                Error_Log("send: ",strerror(errno));
                                exit(0);
                            }
                        }
                    }
                    break;
                }
                else  //如果不在线的话，就启动离线消息机制
                {
                    send_buf.flag=5;
                    time(&now);
                    send_buf.Sendtime=now;
                    strcpy(send_buf.Sendname,"system");
                    strcpy(send_buf.Recvname,buf->Sendname);
                    strcpy(send_buf.Message,"           Offline Message");
                    if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                    {
                        Error_Log("send: ",strerror(errno));
                        exit(0);
                    }
                    if(!Offline_Message_Save(buf)) //保存离线消息并判断是否保存成功
                    {
                        Error_Log("Offline_Message_Save: ","Offline Message Write Fail");
                        return 0;
                    }
                }
            }
            else //未注册的话，报告用户没有此人
            {
                send_buf.flag=5;
                time(&now);
                send_buf.Sendtime=now;
                strcpy(send_buf.Sendname,"system");
                strcpy(send_buf.Recvname,buf->Sendname);
                sprintf(send_buf.Message,"%s:%s","Not Found This Name",buf->Recvname);
                if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send: ",strerror(errno));
                    exit(0);
                }
                return 0;
            }
            break;
        case 7: //修改密码
            if(Change_Password_Srv(buf)) //修改密码函数
            {
                send_buf.flag=6;
                time(&now);
                send_buf.Sendtime=now;
                strcpy(send_buf.Sendname,"system");
                strcpy(send_buf.Recvname,buf->Sendname);
                strcpy(send_buf.Message,"Change Password Success!");
                
                if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send",strerror(errno));
                    exit(0);
                }
            }
            else //修改密码失败的话，报告用户
            {
                send_buf.flag=6;
                time(&now);
                send_buf.Sendtime=now;
                strcpy(send_buf.Sendname,"system");
                strcpy(send_buf.Recvname,buf->Sendname);
                strcpy(send_buf.Message,"Original Password Incorrect,Change Password fail!");
                
                if(send(conn_fd,&send_buf,sizeof(message_node_t),0)<0)
                {
                    Error_Log("send",strerror(errno));
                    exit(0);
                }
            }
            break;
    }
}

int main() //主函数
{
    int sock_fd;
    int conn_fd;
    int optval;
    int ret;
    int srv_len,clt_len;
    int flag_recv=0;
    pid_t pid;
    struct sockaddr_in srv_sock,clt_sock;
    message_node_t recv_buf;
    online_node_t *p;
    System_Init();
    List_Init(head,online_node_t);
    fd_set readfds,testfds;
    
    sock_fd=socket(AF_INET,SOCK_STREAM,0); //创建一个TCP 套接字
    
    if(sock_fd<0)
    {
        Error_Log("socket: ",strerror(errno));
    }

    //设置该套接字使之可以重新绑定
    
    optval=1;
    
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(int))<0)
    {
        Error_Log("setsockopt: ",strerror(errno));
    }
    
    //初始化本地端               
    memset(&srv_sock,0,sizeof(struct sockaddr_in ));
    
    srv_sock.sin_family=AF_INET;
    srv_sock.sin_port=htons(SERV_PORT);
    srv_sock.sin_addr.s_addr=htonl(INADDR_ANY);
    
    //绑定套接字到本地端
    if(bind(sock_fd,(struct sockaddr *)&srv_sock,sizeof(struct sockaddr_in ))<0)
    {
        Error_Log("bind: ",strerror(errno));
    }
    
    //设置监听
    if(listen(sock_fd,MAX_LIST)<0)
    {
        Error_Log("listen: ",strerror(errno));
    }
    
    p=(online_node_t *)malloc(sizeof(online_node_t));
    strcpy(p->name,"system");
    p->sock_fd=sock_fd;
    strcpy(p->address,"127.0.0.1");
    List_AddHead(head,p);  //往在线用户链表里加入系统套接字
    fd_count++;
    srv_len=sizeof(struct sockaddr_in);
    FD_ZERO(&readfds);          //将监听集合清空
    FD_SET(sock_fd,&readfds);   //然后将系统套接字加入到监听集合里 
    while(1)
    {
        int fd;
        int nread;
        int j;
        int flag=0;
        online_node_t *s;
        s=head;
        testfds=readfds; //每次将集合先保存
        
        printf("service waiting\n"); 
        
        ret=select(MAX_LIST, &testfds ,(fd_set *)0,(fd_set *)0,(struct timeval *)0); // IO 复用函数
        if(ret<0)
        {
            Error_Log("select: ",strerror(errno));
        }
        for(fd=0;fd<fd_count;fd++)
        {
            if(flag!=1)
                s=s->next;
            flag=0;
            if(FD_ISSET(s->sock_fd,&testfds))  //检测出现响应的在不在在线用户链表中，不在责不进行操作，直接跳过
            {
                if(s->sock_fd==sock_fd) //如果响应的是监听套接字，则说明是一个新的用户请求
                {
                    clt_len=sizeof(struct sockaddr_in);
                    conn_fd=accept(s->sock_fd,(struct sockaddr *)&clt_sock,&clt_len);       //接受客户端连接
                    printf("New connect %d ip is %s",conn_fd,inet_ntoa(clt_sock.sin_addr)); 
                    int sign=0;
                    char newName[21];
                    sign=Log_Service(conn_fd,newName,inet_ntoa(clt_sock.sin_addr)); //然后进行连接后的处理，注册或登录验证
                    if(sign==1)
                    {
                        p=(online_node_t *)malloc(sizeof(online_node_t));
                        strcpy(p->name,newName);
                        p->sock_fd=conn_fd;
                        strcpy(p->address,inet_ntoa(clt_sock.sin_addr));
                        List_AddHead(head,p);                     
                        FD_SET(p->sock_fd,&readfds);
                        fd_count++;
                        printf("adding client on fd %d name:%s\n",p->sock_fd,p->name);
                        if((pid=fork())==0) //用户上线后先给其发送离线消息（如果有的话）
                        {
                            Offline_Message_Send(p->sock_fd,p->name); //发送离线消息
                            exit(0);
                        }
                        if((pid=fork())==0)     //给所有在线用户发送用户上线提醒
                        {
                            message_node_t buf;
                            online_node_t *t;
                            int j;
                            t=head;
                            time_t now;
                            time(&now);
                            buf.Sendtime=now;
                            buf.flag=3;
                            strcpy(buf.Sendname,"system");
                            strcpy(buf.Recvname,"everyone");
                            strcpy(buf.Message,newName);
                            strcat(buf.Message," Little Friend Online,Hurry To Chatting With Him/Her !");
                            for(j=0;j<fd_count;j++)
                            {          
                                t=t->next;
                                if (t->sock_fd==head->prev->sock_fd || !strcmp(t->name,newName))
                                    continue;
                                if(send(t->sock_fd,&buf,sizeof(message_node_t),0)<0)
                                {
                                    Error_Log("send: ",strerror(errno));
                                    exit(0);
                                }   
                            }
                            exit(0);
                        }
                    }
                }
                else  //否则，表示是已在线的用户出现操作
                {
                    memset(&recv_buf,0,sizeof(message_node_t));  //每次先将缓冲区清空
                    nread = recv(s->sock_fd,&recv_buf,sizeof(message_node_t),0); //接受用户发送来的数据
                    if(nread==0) //如果数据长度为零，则表示用户离开
                    {
                        char OffName[21];
                        online_node_t *r;
                        r=s;
                        s=s->next;
                        flag=1;
                        strcpy(OffName,r->name);
                        FD_CLR(r->sock_fd,&readfds);
                        close(r->sock_fd);                
                        Register_Log(2,r->name,r->address,"log out");
                        printf("removeing clinet on fd %d\n",r->sock_fd);
                        List_FreeNode(r);
                        fd_count--;
                        //创建一个新的线程，用于给所有用户发送用户离线提醒
                        if((pid=fork())==0)
                        {
                            message_node_t buf;
                            online_node_t *t;
                            int j;
                            t=head;
                            time_t now;
                            time(&now);
                            buf.Sendtime=now;
                            buf.flag=3;
                            strcpy(buf.Sendname,"system");
                            strcpy(buf.Recvname,"everyone");
                            strcpy(buf.Message,OffName);
                            strcat(buf.Message," Little Friend Offline!");
                            for(j=0;j<fd_count;j++)
                            {          
                                t=t->next;
                                if (t->sock_fd==head->prev->sock_fd )
                                    continue;
                                if(send(t->sock_fd,&buf,sizeof(message_node_t),0)<0)
                                {
                                    Error_Log("send: ",strerror(errno));
                                    exit(0);
                                }   
                            }
                            exit(0); //发送完成后，进程关闭
                        }
                    }
                    else 
                    {
                        if((pid=fork())==0)  //创建一个新的进程，用于处理用户发来的信息，解析并转发给其他用户
                        {
                            Send_Message(s->sock_fd,&recv_buf);
                            exit(0);
                        }
                    }
                }
            }
        }
    }
}
