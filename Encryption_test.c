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
    char password1[21];
    char password2[21];
    gets(password1);
    printf("%s\n",password1);
    password_encryption(password1);
    gets(password2);
    printf("%s\n",password2);
    password_encryption(password2);
    if(strcmp(password1,password2))
    {
        printf("Diff\n");
    }
    else
    {
        printf("same\n");
    }

}
