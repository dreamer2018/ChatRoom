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
#include<pthread.h>
#include"my_error.h"
#include"Persist.h"


#define STRMAX 100


#define BUFMAX 1024

//函数声明部分
char UserName[21];
int exit_sign=0;



int Chatting_Function(int conn_fd);
int Login_Service(int sign,int argc,char *argv[]);
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
        printf("\t\t=+          1.     Register Now                       +=\n");  //注册入口
        printf("\t\t=+          2.     Sign     In                        +=\n");  //登录入口
        printf("\t\t=+          3.        exit                            +=\n");  
        printf("\t\t=+                                                    +=\n");  
        printf("\t\t======================================================+=\n");
        printf("\t\tPlease Input Your Choice :");
        ch=getch();
        switch(ch)
        {
            case '1':
                sign=1;
                system("clear");
                Login_Service(sign,argc,argv);
                break;
            case '2':
                sign=2;
                system("clear");
                Login_Service(sign,argc,argv);
                break;
            case '3':
                flag=0;
                break;
        }

    }while(flag);
    printf("\n");
}
int Chatting_Service()
{
    int flag=1;
    char ch;
    do
    {
        system("clear");
	    printf("\t\t========================================================\n");
	    printf("\t\t=++++++++++++++++++++++++++++++++++++++++++++++++++++++=\n");
	    printf("\t\t=+               Chatting Service                     +=\n");
	    printf("\t\t=+----------------------------------------------------+=\n");
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t=+          1.     Chat       Now                     +=\n");  //开始聊天
	    printf("\t\t=+          2.  Group  Chat  Records                  +=\n");  //群聊记录
	    printf("\t\t=+          3.  Private Chat Records                  +=\n");  //私聊记录
	    printf("\t\t=+          4. Return  Previous  Step                 +=\n");  //返回上一步
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t========================================================\n");
	    printf("\t\tPlease Input Your Choice :");
	    ch=getch();
	    switch(ch)
	    {
	    	case '1':
		        system("clear");
                return 1;
            case '2':
		            ;
		        break;
		    case '3':
                break; 
            case '4':
		        return 0;       
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
    char buf[BUFMAX];
    while(1)
    {
        printf("Please Input Your Nickname:");
        scanf("%s",buf);
        getchar();
        if(strlen(buf)<21)
        {
            strncpy(Nickname,buf,20);
            Nickname[20]='\0';
            break;
        }
        else
        {
            printf("\033[35mYour Nickname length more than 20,Please Input Again !\033[0m\n\n");
        }
    }
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
}

int Register(int sock_fd,struct sockaddr_in serv_addr)
{
    int rtn=0;
    int i;
    message_node_t recv_buf;
    message_node_t send_buf;
    time_t now; 
    char Nickname[21],Password[21];
    
    Get_info(Nickname,Password);
    memset(&send_buf,0,sizeof(message_node_t));
    send_buf.flag=1;
    strcpy(send_buf.Sendname,Nickname);
    strcpy(send_buf.Recvname,Password);
    strcpy(send_buf.Message,"Register");
    time(&now);
    send_buf.Sendtime=now;
    
    if(connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))<0)
    {
        perror("connect");
        exit(1);
    }
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
        printf("Register Success ! Please Login.....\n");
        sleep(3);
        rtn=1;
    }
    else
    {
       // close(sock_fd);
        printf("%s\n",recv_buf.Message);
        close(sock_fd);
    }
    return rtn;
}

int Sign_In(int sock_fd,struct sockaddr_in serv_addr,message_node_t *recv_buf)
{
    int i;
    int rtn=0;
    char Nickname[21],Password[21];
    time_t now;
    message_node_t send_buf;
    printf("Please Input Your Nickname:");
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
    if(connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))<0)
    {
        perror("connect");
        exit(1);
    }

 
    if(send(sock_fd,&send_buf,sizeof(message_node_t),0)<0)
    {
        perror("send");
        exit(0);
    }
        
    if(recv(sock_fd,recv_buf,sizeof(message_node_t),0)<0)
    {
        perror("send");
        exit(0);
    }
        
    if(!strncmp(recv_buf->Message,"Succ",4))
    {
        strcpy(UserName,recv_buf->Recvname);
        rtn=1;
    }
    else
    {
        close(sock_fd);
    }
    return rtn;
}

void print_time(time_t st_time) //日期解析函数
{
    struct tm *p;
    p=localtime(&st_time);
    printf("%d-",p->tm_year+1900);
    if(p->tm_mon<10)
    {
        printf("0%d-",p->tm_mon+1);
    }
    else
    {
        printf("%2d-",p->tm_mon+1);
    }
    if(p->tm_mday<10) //小于十的前面补零
    {
        printf("0%d",p->tm_mday); 
    }
    else
    {
        printf("%2d",p->tm_mday);        
    }
    if(p->tm_hour<10)
    {
        printf(" 0%d:",p->tm_hour);        
    }
    else
    {
       printf(" %2d:",p->tm_hour);        
    }
    if(p->tm_min<10)
    {
        printf("0%d",p->tm_min);        
    }
    else
    {
        printf("%2d",p->tm_min);           
    }
    if(p->tm_sec<10)
    {
        printf(":0%d\n",p->tm_sec);
    }
    else
    {
        printf(":%2d\n",p->tm_sec);
    }
}

int Login_Service(int sign,int argc,char *argv[])
{   
    int i;
    int ret;
    int conn_fd;
    int serv_port;
    int *clientfdp;
    int status;
    pthread_t tid1,tid2;
    int flag;
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

    if(sign==1)
    {
        while(1)
        {
            conn_fd=socket(AF_INET,SOCK_STREAM,0);
            if(Register(conn_fd,serv_addr))
            {
                return 0;
            }
        }
    }
    else if(sign==2)
    {
        message_node_t buf;
        for(i=2;i>=0;i--)
        {
            conn_fd=socket(AF_INET,SOCK_STREAM,0);
            if(Sign_In(conn_fd,serv_addr,&buf))
            {
                Client_Init(UserName);
                break;
            }
            else
            {
                printf("\033[35m%s\033[0m\n",buf.Message);
                if(i>0)
                {
                    printf("You Have %d Chance,Please Try Again\n\n",i);   
                }
                else
                {
                    printf("Entered An Incorrect Password Three Times,To Quit !\n\n");
                    sleep(3);
                    return -1;
                }
            }
            memset(&buf,0,sizeof(message_node_t));
        }
    }
    if(Chatting_Service())
    {
        pthread_create(&tid1,NULL,threadsend,&conn_fd);
        pthread_create(&tid2,NULL,threadrecv,&conn_fd);
        pthread_join(tid2,(void *)&status);
    }
    else
    {
        close(conn_fd);
        return 0;
    }
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
        memset(temp,0,BUFSIZE);
        memset(&buf,0,sizeof(message_node_t));
        gets(temp);
        time(&now);
        if(strlen(temp)>512)
        {
            printf("Send Fail,Message Too Long\n");
            continue;
        }
        printf("      send  Success! ");
        print_time(now);
        printf("\n");
        if(!strncmp(temp,"@",1))
        {
            int i,j=-1,k=-1;
            for(i=1;j<20;i++)
            {
                if(temp[i]==' ')
                    break;
                j++;
                buf.Recvname[j]=temp[i];
            }
            buf.Recvname[j+1]='\0';
            for(j=i;j<512;j++)
            {
                if(temp[j]==' ')
                    continue;
                while(1)
                {
                    k++;
                    buf.Message[k]=temp[j];
                    j++;
                    if(temp[j]=='\0')
                        break;
                }
                break;
            }
            buf.flag=4;
            strcpy(buf.Sendname,UserName);
            buf.Sendtime=now;
            
            if(send(connfd,&buf,sizeof(message_node_t),0)<0)
            {
                perror("send");
            }
            Client_Message_Save(UserName,&buf);
        }
        else if(!strncmp(temp,"//",2))
        {
            int i,j=-1,k=-1;
            buf.flag=0;
            for(i=2;;i++)
            {
                j++;
                buf.Message[j]=temp[i];
                if(temp[i]=='\0')
                    break;
            }
            buf.flag=0;
            strcpy(buf.Sendname,UserName);
            strcpy(buf.Recvname,"system");
            buf.Sendtime=now;
            if(!strncmp(buf.Message,"quit",4))
            {
                exit_sign=1;
                if(send(connfd,&buf,sizeof(message_node_t),0)<0)
                {
                    perror("send");
                }
                close(connfd);
                pthread_exit(0); 
            }
            if(send(connfd,&buf,sizeof(message_node_t),0)<0)
            {
                perror("send");
            }
        }
        else
        {
            buf.flag=3;
            strcpy(buf.Sendname,UserName);
            buf.Sendtime=now;
            strcpy(buf.Recvname,"everyone"); 
            strcpy(buf.Message,temp);
            if(send(connfd,&buf,sizeof(message_node_t),0)<0)
            {
                perror("send");
            }
            Client_Group_Message_Save(UserName,&buf);
        }
    }
    return NULL;
}


void *threadrecv(void *vargp)
{
    int connfd = *((int *)vargp);   
    message_node_t buf;
    while(1)
    {
        memset(&buf,0,sizeof(message_node_t));
        int idata = 0;
        idata = recv(connfd,&buf,sizeof(message_node_t),0);
        if(idata > 0)
        {
            if(exit_sign==1 && !strncmp(buf.Message,"quit",4))
            {
                pthread_exit(0);
            }
            printf("\n");
            print_time(buf.Sendtime);
            if(buf.flag==4)
            {
                printf("\033[45m%-6s\033[0m :\033[35m %s\033[0m\n",buf.Sendname,buf.Message);
                Client_Message_Save(UserName,&buf);
            }
            else
            {
                printf("\033[34m%-6s\033[0m : %s\n",buf.Sendname,buf.Message);
                Client_Group_Message_Save(UserName,&buf);
            }
        }
    }
    return NULL;
}
