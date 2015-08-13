/*************************************************************************
	> File Name: system_test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Thu 13 Aug 2015 05:00:20 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main()
{
    int a=system("touch /");
    printf("system teturn :%d",a);
}
