/*************************************************************************
	> File Name: tinme_test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Sat 08 Aug 2015 09:28:51 AM CST
 ************************************************************************/
#include<stdio.h>
#include<time.h>
#include<unistd.h>
void  print_time(time_t st_time );
int main()
{
    int i;
    time_t now;
    char nowtime[30];
    //for(i=0;i<5;i++)
    //{
        time(&now);
        //ctime_r(&now,nowtime);
    //    printf("%s",nowtime);
        print_time(now);
      //  sleep(2);
    //}
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
