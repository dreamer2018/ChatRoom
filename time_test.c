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
    //{
        time(&now);
        print_time(now);
}

void print_time(time_t st_time) //日期解析函数
{
    struct tm *p;
    p=localtime(&st_time);
    printf("%d-%.2d-%.2d %.2d:%.2d:%.2d",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
}
