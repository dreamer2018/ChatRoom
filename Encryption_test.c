/*************************************************************************
	> File Name: Encryption_test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Tue 18 Aug 2015 09:28:44 AM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>

void password_encryption(char *password)
{
    int i;
    for(i=0;i<strlen(password);i++)
    {
        password[i]+=i;
    }
}

int main()
{
    char password[21];
    gets(password);
    printf("%s\n",password);
    password_encryption(password);
    printf("%s\n",password);
}
