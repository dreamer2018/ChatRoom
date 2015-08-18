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
int Client_Init(char *user_name)
{ 
    char path_1[40]="./";
    char path_2[40]="./";
    char file_1[10]="/Group/";
    char file_2[10]="/Private/";
    char file[9]="Chat.dat";
    char command1[50]="mkdir ";
    char command2[50]="mkdir ";
    char command3[50]="mkdir ";
    char command4[50]="touch ";
    char command5[50]="touch ";
    strcat(path_1,user_name);
    strcat(path_2,user_name);
    if(!Exists(path_1))  //在当前目录下创建用户名为名的文件夹 ./user_name
    {
        strcat(command1,path_1);
        system(command1);
    }

    strcat(path_1,file_1);      //  ./user_name/Group/
    strcat(path_2,file_2);      // ./user_name/Private/
    
    if(!Exists(path_1))         //创建./user_name/Group/
    {     
        strcat(command2,path_1);
        system(command2); 
    }

    if(!Exists(path_2))         //创建./user_name/Private/
    {    
        strcat(command3,path_2);
        system(command3);
    }
    
    strcat(path_1,file);
    strcat(path_2,file);

    if(!Exists(path_1))
    {
        strcat(command4,path_1);
        system(command4);
    }
    
    if(!Exists(path_2))
    {
        strcat(command5,path_2);
        system(command5);
    }
}
int main()
{
    //System_Init();
   // User_Init("zhoufre");
    //Client_Init("adminfffffffffffffff");
    //system("cat file_test.c");
    //    rename("./hehe.txt","./lala.txt");
    remove("./lala.txt");
}

