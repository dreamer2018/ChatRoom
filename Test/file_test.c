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


int Exists(char *path)
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

int System_Init()
{
    int i;
    char path[][20]={"./sys/","./user/","./data/","./user/group","./user/offline"};
    char command[50];
    
    char mkfile[][40]={"touch ./sys/Error.log","touch ./sys/Register.log","touch ./user/group/Chat.dat","touch ./data/UserInfo.dat","touch ./user/offline/OffLine.dat","touch ./sys/Help.ini"};
    
    for(i=0;i<5;i++)
    {
        if(!Exists(path[i]))
        {
            memset(command,0,sizeof(command));
            strcpy(command,"mkdir ");
            strcat(command,path[i]);
            system(command);
        }
    }
    for(i=0;i<6;i++)
    {
        if(!Exists(mkfile[i]))
        {
            system(mkfile[i]);
        }
    }
}
int User_Init(char *user_name)
{
    char path[40]="./user/";
    char file[9]="Chat.dat";
    char command1[50]="mkdir ";
    char command2[50]="touch ";
    strcat(path,user_name);
    if(!Exists(path))
    {
        strcat(command1,path);
        system(command1);
    }
    strcat(path,"/");
    strcat(path,file); 
    if(!Exists(path))
    {
        strcat(command2,path);
        system(command2);
    }
}
int main()
{
    System_Init();
    User_Init("zhoufre");
    system("cat file_test.c");
}

