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
#include"conio.h"
#include"List.h"
#include<time.h>
//#include"Persist.h"


#define STRMAX 100


#define BUFMAX 512

//函数声明部分
char UserName[21];




int Chatting_Function(int flag,int argc,char *argv[]);
void Chat_Records_Query();
void *threadrecv(void *vargp);
void *threadsend(void *vargp);


int main(int argc,char *argv[]) //主函数
{
    char ch;
    int flag=1;
    int sign;
    //判断所输入的命令行参数够不够
    if(argc!=5)
    {
        printf("Usage: [-p] [serv_port] [-a] [serv_address] \n");
        exit(1);
    }

    //主界面，进行一系列操作
    do
    {
        system("clear");
        printf("\t\t========================================================\n");
        printf("\t\t=++++++++++++++++++++++++++++++++++++++++++++++++++++++=\n");
        printf("\t\t=+               Welcome To Chat Room                 +=\n");
        printf("\t\t=+----------------------------------------------------+=\n");
        printf("\t\t=+                                                    +=\n");
        printf("\t\t=+          1.     Register Now                       +=\n");  //注册函数
        printf("\t\t=+          2.     Chat     Now                       +=\n");  //登录聊天函数
        printf("\t\t=+          3.  Look Chatting Records                 +=\n");  //聊天记录查询函数
        printf("\t\t=+          4.        exit                            +=\n");  
        printf("\t\t=+                                                    +=\n");  
        printf("\t\t======================================================+=\n");
        printf("\t\tPlease Input Your Choice :");
        ch=getch();
        switch(ch)
        {
            case '1':
                sign=1;
                system("clear");
                Chatting_Function(sign,argc,argv);
                break;
            case '2':
                sign=2;
                system("clear");
                Chatting_Function(sign,argc,argv);
                break;
            case '3':
                Chat_Records_Query();
                break;
            case '4':
                flag=0;
                break;
        }

    }while(flag);
    printf("\n");
}
void Chat_Records_Query()
{
    int flag=1;
    char ch;
    do
    {
        system("clear");
	    printf("\t\t========================================================\n");
	    printf("\t\t=++++++++++++++++++++++++++++++++++++++++++++++++++++++=\n");
	    printf("\t\t=+               Chat Records Query                   +=\n");
	    printf("\t\t=+----------------------------------------------------+=\n");
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t=+          1.  Group  Chat  Records                  +=\n");  //群聊记录
	    printf("\t\t=+          2.  Private Chat Records                  +=\n");  //私聊记录
	    printf("\t\t=+          3. Return  Previous  Step                 +=\n");  //返回上一步
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t========================================================\n");
	    printf("\t\tPlease Input Your Choice :");
	    ch=getch();
	    switch(ch)
	    {
	    	case '1':
		        break;
		    case '2':
		            ;
		        break;
		    case '3':
		        flag=0;
		        break;
		}
	}while(flag);
}

void passwd(char *password)
{
    int i,flag;
    char ch;
    for(i=0;;i++)
    {
        ch=getch();
        if(ch=='\n')
        {
            printf("\n");
            break;
        }
        if(i<20)
        {
            password[i]=ch;
            putchar('*');
        }
    }
    if(i<20)
    {
        password[i]='\0';
    }
    else
    {
        password[20]='\0';
    }
}
void getname(char *name)
{
    int i,flag;
    char ch;
    for(i=0;;i++)
    {
        ch=getch();
        if(ch=='\n')
        {
            printf("\n");
            break;
        }
        if(i<20)
        {
            name[i]=ch;
            putchar(name[i]);
        }
    }
    if(i<20)
    {
        name[i]='\0';
    }
    else
    {
        name[20]='\0';
    }
}


int getpasswd(char *password)
{
    char passwd_first[21];
    char passwd_again[21];

    printf("Please Input Your Password:");
    passwd(passwd_first);
    
    printf("Please Input Your Password Again:");
    passwd(passwd_again);
    
    if(strcmp(passwd_first,passwd_again))
    {
        strcpy(password,"Error\n");
        return 0;
    }
    strcpy(password,passwd_again);
    return 1;
}

void Get_info(char *Nickname,char *Password)
{
    int i,j;
    printf("Please Input Your Nickname:");
    //getname(Nickname);
    scanf("%s",Nickname);
    getchar();
    for(i=0;i<10;i++)
    {
        if(!getpasswd(Password))
        {
            printf("Two Entered Password Diff,Please Try Again!\n");
        }
        else if(strlen(Password)<6)
        {
            printf("This Password Less Than Six Characters!\n");
        }
        else
        {
            break;
        }
    }
    printf("Nickname:%s Password:%s\n",Nickname,Password);
}

int Register(int sock_fd)
{
    int rtn=0;
    int i;
    message_node_t recv_buf;
    message_node_t send_buf;
    time_t now; 
    char Nickname[21],Password[21];
    for(i=0;;i++)
    {
        Get_info(Nickname,Password);
        memset(&send_buf,0,sizeof(message_node_t));
        send_buf.flag=1;
        strcpy(send_buf.Sendname,Nickname);
        strcpy(send_buf.Recvname,Password);
        strcpy(send_buf.Message,"Register");
        time(&now);
        send_buf.Sendtime=now;
        printf("Test\n");
        if(send(sock_fd,&send_buf,sizeof(message_node_t),0)<0)
        {
            perror("send");
            exit(0);
        }
        memset(&send_buf,0,sizeof(message_node_t));
        if(recv(sock_fd,&recv_buf,sizeof(message_node_t),0)<0)
        {
            perror("recv");
            exit(0);
        }
        if(!strncmp(recv_buf.Message,"Succ",4))
        {
            printf("Register Success !\n");
            rtn=1;
            break;
        }
        else
        {
            printf("%s\n",recv_buf.Message);
        }
    }
    return rtn;
}

int Sign_In(int sock_fd)
{
    int i;
    char Nickname[21],Password[21];
    time_t now;
    message_node_t recv_buf,send_buf;
    for(i=0;i<3;i++)
    {
        printf("Please Input Your Nickname:");
        //getname(Nickname);
        scanf("%s",Nickname);
        getchar();
        printf("Please Input Your Password:");
        passwd(Password);
        send_buf.flag=2;
        strcpy(send_buf.Sendname,Nickname);
        strcpy(send_buf.Recvname,Password);
        strcpy(send_buf.Message,"Sign in");
        time(&now);
        send_buf.Sendtime=now;
        if(send(sock_fd,&send_buf,sizeof(message_node_t),0)<0)
        {
            perror("send");
            exit(0);
        }
        
        if(recv(sock_fd,&recv_buf,sizeof(message_node_t),0)<0)
        {
            perror("send");
            exit(0);
        }
        if(!strncmp(recv_buf.Message,"Succ",4))
        {
            printf("Signin Success \n");
            break;
        }
        else
        {
            printf("Nickname Or Password Error\n");
            printf("Please Try Again\n");
        }
    }
}

int Chatting_Function(int sign,int argc,char *argv[])
{   
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    int *clientfdp;
    int status;
    pthread_t tid1,tid2;
    clientfdp=(int *)malloc(sizeof(int));
    struct sockaddr_in serv_addr;
    char recv_buf[BUFMAX];
    char Nickname[21];
    char Password[21];
    //初始化服务器端地址结构
    
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family=AF_INET;
    
    //从命令行获取服务器端的端口和地址
    
    for(i=0;i<argc;i++)
    {
        if(strcmp("-p",argv[i])==0)
        {
            serv_port=atoi(argv[i+1]);  //将字符串转换成整型
            
            if(serv_port<0||serv_port>65535)  //如果端口号在这个范围之外，肯定错误，直接报错
            {
                printf("invalid serv_port\n"); 
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

    printf("connected\n");
    if(sign==1)
    {
        return Register(conn_fd);
    }
    else if(sign==2)
    {
        Sign_In(conn_fd);       
    }

    pthread_create(&tid1,NULL,threadsend,&conn_fd);
    pthread_create(&tid2,NULL,threadrecv,&conn_fd);
    pthread_join(tid1,(void *)&status);
    pthread_join(tid2,(void *)&status);
    close(conn_fd);
    return 0;
}

void *threadsend(void * vargp)
{
    int connfd = *((int *)vargp);
    int idata;
    char temp[BUFMAX];
    message_node_t buf;
    time_t now;
    while(1)
    {
        printf("\033[34m ME:");
        fgets(temp,BUFMAX,stdin);
        //gets(temp);
        printf("\033[0m");
        send(connfd,temp,BUFMAX,0);
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
        printf("\033[36m");
        if(idata > 0)
        {
            printf("server :%s",temp);
        }
        printf("\033[0m");
        //printf("Test\n");
    }
    return NULL;
}
