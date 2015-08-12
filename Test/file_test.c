/*************************************************************************
	> File Name: file_test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Wed 12 Aug 2015 02:33:29 PM CST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int exists(char *path)
{
    FILE *fp;
    //printf("%s\n",path);
    fp=fopen(path,"rb");
    if(fp==NULL)
    {
        return 0;
    }
    else
    {
        fclose(fp);
        return 1;
    }
}
void system_init()
{
    int i;
    char path[][20]={{"./sys/"},{"./user/"},{"./data/"},{"./user/group"}};
    char command[30];
    
    char mkfile[][30]={"touch ./sys/error.log","touch ./sys/register.log","touch ./user/group/Chat.dat","touch ./data/User_info.dat"};
    
    for(i=0;i<4;i++)
    {
        if(!exists(path[i]))
        {
            memset(command,0,sizeof(command));
            strcpy(command,"mkdir ");
            strcat(command,path[i]);
            system(command);
        }
    }
    for(i=0;i<4;i++)
    {
        if(!exists(mkfile[i]))
        {
            system(mkfile[i]);
        }
    }
}
int main()
{
    //FILE *fp;
    //system("mkdir ./Test");
    
    //system("mkdir ./sys/");
    //system("mkdir ./sys/group/");
    system_init();
    /*
    strcpy(path,"/home/zhoupan/heihei/hehe");
    strcat(path2,path);
    if(exists(path))
    {
        printf("存在\n");
    }
    else
    {
        printf("not exist\n");
        system(path2);
    }*/
    //fprintf(fp,"%s\n","我很着急");
    //system("mkdir ~/Project/ChatRoom/Test");
    //system("touch ./Test/hehe");
    
}

