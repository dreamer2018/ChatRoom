/*************************************************************************
	> File Name: string_test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Sat 15 Aug 2015 03:29:47 PM CST
 ************************************************************************/

#include<stdio.h>
int main()
{
    char Recvname[]="hello";
    char Message[50];
    sprintf(Message,"%s:%s","Not Found This Name",Recvname);
    printf("%s\n",Message);
}
