 /*************************************************************************
	> File Name: passwd.c
	> Author: ZhouPan / github:dreamer2018 
	> Blog: blog.csdn.net/it_dream_er 
	> Mail: zhoupans_mail@163.com 
	> Created Time: Fri 07 Aug 2015 03:39:41 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"conio.h"

#define STRMAX 100

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

int main()
{
    int i,j;
    char Nickname[21];
    char Password[21];
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
            printf("Register Successfully!\n");
            break;
        }
    }
    printf("%s %s \n",Nickname,Password);
}
