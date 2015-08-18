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
#define PLAY_PAGE_SIZE 10

#define BUFMAX 1024

//函数声明部分
char UserName[21];
int exit_sign=0;

int Chatting_Function(int conn_fd);
int Login_Service(int sign,int argc,char *argv[]);
void *threadrecv(void *vargp);
void *threadsend(void *vargp);
void print_time(time_t st_time); //日期解析函数
void Get_Group_Chatting_Record(int fla,date_t dt1,date_t dt2,char *name);
void Get_Private_Chatting_Record(int flag,date_t dt1,date_t dt2,char *name);
int DateCmp();
int DateCmp_Srv();
int Chat_Record_Srv(int sign);  //sign=1 私聊 sign=0 群聊
void Change_Password(int sock_fd);  //修改密码函数

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
int Chatting_Service(int conn_fd)
{
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
	    printf("\t\t=+          4.    Change Password                     +=\n");  //私聊记录
	    printf("\t\t=+          5. Return  Previous  Step                 +=\n");  //返回上一步
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
                system("clear");
		        Chat_Record_Srv(0);
                break;
		    case '3':
                system("clear");
                Chat_Record_Srv(1);
                break; 
            case '4':
		        Change_Password(conn_fd);
                break;
            case '5':
                return 0;
		}
	}while(1);
}

int Chat_Record_Srv(int sign)  //sign=1 私聊 sign=0 群聊
{
    char ch;
    char name[21];
    date_t dt1,dt2;
    do
    {
        system("clear");
	    printf("\t\t========================================================\n");
	    printf("\t\t=++++++++++++++++++++++++++++++++++++++++++++++++++++++=\n");
	    printf("\t\t=+               Chatting Service                     +=\n");
	    printf("\t\t=+----------------------------------------------------+=\n");
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t=+          1.      Look At All                       +=\n");  //群聊记录
	    printf("\t\t=+          2.    Look At By Time                     +=\n");  //私聊记录
	    printf("\t\t=+          3.    Only See Person                     +=\n");  //私聊记录
	    printf("\t\t=+          4. Return  Previous  Step                 +=\n");  //返回上一步
	    printf("\t\t=+                                                    +=\n");
	    printf("\t\t========================================================\n");
	    printf("\t\tPlease Input Your Choice :");
	    ch=getch();
	    switch(ch)
	    {
	    	case '1':
		        system("clear");
                if(sign==1)
                {
                    Get_Private_Chatting_Record(0,dt1,dt2,name);  //查看全部为0，按时间查为1，按名查为2，
                }
                else
                {
                    Get_Group_Chatting_Record(0,dt1,dt2,name);
                }
                break;
            case '2':
                system("clear");
                printf("Early Time(eg:2015 08 15 10 15):");
                scanf("%d %d %d %d %d",&dt1.year,&dt1.month,&dt1.day,&dt1.hour,&dt1.minute);
                getchar();
                if(dt1.month>12 || dt1.month<=0 || dt1.day>31 || dt1.day<=0 || dt1.hour<0||dt1.hour>24 ||dt1.minute<0||dt1.minute>60)
                {
                    printf("\033[35mThe Time You Inter Not Correct !\033[0m\n");
                    sleep(3);
                    continue;
                }
                printf("Later Time(eg:2015 08 15 11 20):");
                scanf("%d %d %d %d %d",&dt2.year,&dt2.month,&dt2.day,&dt2.hour,&dt2.minute);
                getchar();
                dt1.second=0;
                dt2.second=0;
                if(dt2.month>12 || dt2.month<=0 || dt2.day>31 || dt2.day<=0 || dt2.hour<0||dt2.hour>24 ||dt2.minute<0||dt2.minute>60)
                {
                    printf("\n\033[35mThe Time You Inter Not Correct !\033[0m\n");
                    sleep(3);
                    continue;
                }
                if(DateCmp(dt1,dt2)==1)
                {
                    printf("\n\033[35mEarly Time Must Sooner than Later Time !\033[0m\n");
                    sleep(3);
                    continue;
                }
                if(sign==1)
                {
                    Get_Private_Chatting_Record(1,dt1,dt2,name);  //查看全部为0，按时间查为1，按名查为2，
                }
                else
                {
                    Get_Group_Chatting_Record(1,dt1,dt2,name);
                }
		        break;
		    case '3':
                system("clear");
                printf("Input You Need Look Up Person Name:");
                scanf("%s",name); 
                if(sign==1)
                {
                    Get_Private_Chatting_Record(2,dt1,dt2,name);  //查看全部为0，按时间查为1，按名查为2，
                }
                else
                {
                    Get_Group_Chatting_Record(2,dt1,dt2,name);
                }
		        break;               
            case '4':
		        return 0;       
		}
    }while(1);
}
void Get_Private_Chatting_Record(int sign,date_t dt1,date_t dt2,char *name)
{
	int i;
    char choice;
	message_node_t *head;
	message_node_t *pos;
	Pagination_t paging;
	List_Init(head,message_node_t);
	paging.offset=0;
	paging.pageSize = PLAY_PAGE_SIZE;
	if(sign==1)
    {
        paging.totalRecords = Private_Message_SelectByTime(UserName,dt1,dt2,head);
    }
    else if(sign==2)
    {
        paging.totalRecords = Private_Message_SelectByName(UserName,name,head);
    }
    else
    {
        paging.totalRecords = Private_Message_SelectAll(UserName,head);
    }
	Paging_Locate_FirstPage(head, paging);
	do {
		system("clear");
		printf("\n\t\t\t\033[36m===================================================================\n");
		printf("\t\t\t********************* Group Chat Records **************************\033[0m\n");
		Paging_ViewPage_ForEach(head, paging, message_node_t , pos, i)
        {
            if(sign==2)
            {
		        printf("\t\t\t\033[34m");
		        print_time(pos->Sendtime); //日期解析函数
		        printf("\033[0m");
	            printf("\t\t\t\033[35m%s\033[0m:%s\n",pos->Sendname,pos->Message);  
            }
            else
            {
    	        if(!strcmp(pos->Sendname,UserName))
	            {
		            printf("\t\t\t\033[34m");
		            print_time(pos->Sendtime); //日期解析函数
		            printf("\033[0m");
    	            printf("\t\t\t\033[35m%s\033[0m:%s\n",pos->Sendname,pos->Message);
	    	    }
                else
	            {
		            printf("\t\t\t\033[34m");
    		        printf("%48s"," ");
	                print_time(pos->Sendtime); //日期解析函数
	                printf("\033[0m");
		            printf("\t\t\t%58s\033[35m:%s\033[0m\n",pos->Message,pos->Sendname);     
		        }
		    }
        }
		printf("\t\t\t\033[36m==================================================================\n");
		printf("\t\t\t-------- Total Records:%2d  -------------------- 页数: %2d/%2d ------\n",
paging.totalRecords, Pageing_CurPage(paging),Pageing_TotalPages(paging));
		printf("\t\t\t******************************************************************\n");
		printf("\t\t\t**        [P]   上一页      [N]  下一页       [R]  返回       　**\n");
        printf("\t\t\t******************************************************************\033[0m\n");
		choice=getch();
        switch (choice) 
		{
    		case 'p':	
	    	case 'P':
		    	if (!Pageing_IsFirstPage(paging))
			    {
				    Paging_Locate_OffsetPage(head, paging, -1, message_node_t);
    			}
	    		break;
		    case 'n':
    		case 'N':
	    		if (!Pageing_IsLastPage(paging)) 
		    	{
			    	Paging_Locate_OffsetPage(head, paging, 1, message_node_t);
    			}
	    		break;
	    }
    }while(choice !='r' && choice != 'R');
	List_Destroy(head,message_node_t);
}

void Get_Group_Chatting_Record(int sign,date_t dt1,date_t dt2,char *name)
{
	int i;
    char choice;
	message_node_t *head;
	message_node_t *pos;
	Pagination_t paging;
	List_Init(head,message_node_t);
	paging.offset=0;
	paging.pageSize = PLAY_PAGE_SIZE;
	if(sign==1)
    {
        paging.totalRecords = Group_Message_SelectByTime(UserName,dt1,dt2,head);
    }
    else if(sign==2)
    {
        paging.totalRecords = Group_Message_SelectByName(UserName,name,head);
    }
    else
    {
        paging.totalRecords = Group_Message_SelectAll(UserName,head);
    }
    Paging_Locate_FirstPage(head, paging);
	do {
		system("clear");
		printf("\n\t\t\t\033[36m===================================================================\n");
		printf("\t\t\t********************* Group Chat Records **************************\033[0m\n");
		Paging_ViewPage_ForEach(head, paging, message_node_t , pos, i)
        {   
	        if(sign==2)
            {
		        printf("\t\t\t\033[34m");
		        print_time(pos->Sendtime); //日期解析函数
	            printf("\033[0m");
	            printf("\t\t\t\033[35m%s\033[0m:%s\n",pos->Sendname,pos->Message);  
            }
            else
            {
                if(!strcmp(pos->Sendname,UserName))
	            {
		            printf("\t\t\t\033[34m");
		            print_time(pos->Sendtime); //日期解析函数
    	            printf("\033[0m");
	                printf("\t\t\t\033[35m%s\033[0m:%s\n",pos->Sendname,pos->Message);
		        }
	            else
    		    {
	    	        printf("\t\t\t\033[34m");
                    printf("%48s"," ");
	                print_time(pos->Sendtime); //日期解析函数
	                printf("\033[0m");
    		        printf("\t\t\t%58s\033[35m:%s\033[0m\n",pos->Message,pos->Sendname);     
	    	    }
		    }
        }
		printf("\t\t\t\033[36m==================================================================\n");
		printf("\t\t\t-------- Total Records:%2d  -------------------- 页数: %2d/%2d ------\n",
paging.totalRecords, Pageing_CurPage(paging),Pageing_TotalPages(paging));
		printf("\t\t\t******************************************************************\n");
		printf("\t\t\t**        [P]   上一页      [N]  下一页       [R]  返回       　**\n");
        printf("\t\t\t******************************************************************\033[0m\n");
		choice=getch();
		switch (choice) 
		{
    		case 'p':	
	    	case 'P':
		    	if (!Pageing_IsFirstPage(paging))
			    {
				    Paging_Locate_OffsetPage(head, paging, -1, message_node_t);
    			}
	    		break;
		    case 'n':
    		case 'N':
	    		if (!Pageing_IsLastPage(paging)) 
		    	{
			    	Paging_Locate_OffsetPage(head, paging, 1, message_node_t);
    			}
	    		break;
	    }
    }while(choice !='r' && choice != 'R');
	List_Destroy(head,message_node_t);
}

void passwd(char *password)
{
    int i=0,flag;
    char ch;

    system("stty -echo");
    system("stty -icanon");
    
    while(1)
    {
        ch=getchar();
        if(ch=='\n')
        {
            printf("\n");
            break;
        } 
        
        if(ch==127)
        {
            if(i>=1)
            {
                printf("\b");
                printf(" ");
                printf("\b");
                i--;
                continue;
            }
        }
        else
        {
            if(i<20)
            {
                password[i]=ch;
                putchar('*');
                i++;
            }
        }
    }

    system("stty icanon");
    system("stty echo");
    
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
    int i=0,flag;
    char ch;
    
    system("stty -echo");
    system("stty -icanon");
    
    while(1)
    {
        ch=getchar();
        if(ch=='\n')
        {
            printf("\n");
            break;
        } 
        
        if(ch==127)
        {
            if(i>=1)
            {
                printf("\b");
                printf(" ");
                printf("\b");
                i--;
                continue;
            }
        }
        else
        {
            if(i<20)
            {
                name[i]=ch;
                putchar(ch);
                i++;
            }
        }
    }

    system("stty icanon");
    system("stty echo");
    
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

void Change_Password(int conn_fd)
{
    int i;
    message_node_t buf;
    time_t now;
    char Nickname[21];
    char Origin_Password[21];
    char New_Password[21];
    printf("Please Input Original Password:");
    passwd(Origin_Password);
    for(i=0;i<10;i++)
    {
        if(!getpasswd(New_Password))
        {
            printf("Two Entered Password Diff,Please Try Again!\n");
        }
        else if(strlen(New_Password)<6)
        {
            printf("This Password Less Than Six Characters!\n");
        }
        else
        {
            break;
        }
    }
    buf.flag=7;
    time(&now);
    buf.Sendtime=now;
    strcpy(buf.Sendname,UserName);
    strcpy(buf.Recvname,New_Password);
    strcpy(buf.Message,Origin_Password);
    if(send(conn_fd,&buf,sizeof(message_node_t),0)<0)
    {
        perror("send");
    }
}
void Get_info(char *Nickname,char *Password)
{
    int i,j;
    char buf[BUFMAX];
    printf("Please Input Your Nickname:");
    getname(Nickname);
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
    getname(Nickname);
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
    printf("%d-%.2d-%.2d %.2d:%.2d:%.2d\n",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
}

void Simply_time(time_t st_time)
{
    struct tm *p;
    p=localtime(&st_time);
    printf("%.2d:%.2d:%.2d",p->tm_hour,p->tm_min,p->tm_sec);
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
    if(Chatting_Service(conn_fd))
    {
        printf("***************************Begin Chatting***********************************\n");
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
        printf("      send success! ");
        Simply_time(now);
        printf("\n\n");
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
            if(buf.flag==4)
            {
                printf("\n%68s"," ");
                Simply_time(buf.Sendtime);
                printf("\n");
                printf("\033[35m%65s\033[0m :\033[45m %8s\033[0m\n",buf.Message,buf.Sendname);
                Client_Message_Save(UserName,&buf);
            }
            else if(buf.flag==5)
            {
                printf("    \033[36m%s\033[0m\n",buf.Message);
                Client_Message_Save(UserName,&buf);
            }
            else if(buf.flag==6)
            {
                printf("\033[36m%s\033[0m\n",buf.Message);
            }
            else
            {
                printf("\n%68s"," ");
                Simply_time(buf.Sendtime);
                printf("\n");
                printf("%65s : \033[34m%8s\033[0m\n",buf.Message,buf.Sendname);
                Client_Group_Message_Save(UserName,&buf);
            }
        }
    }
    return NULL;
}
