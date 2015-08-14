/*************************************************************************
	> File Name: Persist.h
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Sun 09 Aug 2015 11:28:43 AM CST
 ************************************************************************/

#ifndef _PERSIST_H
#define _PERSIST_H

static const char USERINFO_DATA_FILE[] = "./data/UserInfo.dat";
static const char GROUPCHAT_DATA_FILE[] ="./user/group/Chat.dat";

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
int Register_Persist(message_node_t *data)  //用户信息写入函数,返回1 表示操作成功，0 表示操作失败
{
    int rtn=0;
	FILE *fp;
	fp=fopen(USERINFO_DATA_FILE,"ab");
	if(fp==NULL)
	{
		printf("文件打开失败！");
	}
	else
	{
		fwrite(data,sizeof(message_node_t),1,fp);
		rtn=1;
	}
	fclose(fp);
	return rtn;
}

int Service_Group_Message_Save(char *file,message_node_t *data)  //用户信息写入函数,返回1 表示操作成功，0 表示操作失败
{
    int rtn=0;
	FILE *fp;
	fp=fopen(file,"ab");
	if(fp==NULL)
	{
		printf("File Open Fail !\n");
    }
	else
	{
		fwrite(data,sizeof(message_node_t),1,fp);
		rtn=1;
	}
	fclose(fp);
	return rtn;
}

int Service_Message_Save(char *name,char *file,message_node_t *data)
{
    char path[50];
    int rtn=0;
	FILE *fp;
    strcpy(path,file);
    strcat(path,name);
    strcat(path,"/Chat.dat");
	fp=fopen(path,"ab");
	if(fp==NULL)
	{
        Error_Log(path,":This File Open Fail!");
    }
	else
	{
		fwrite(data,sizeof(message_node_t),1,fp);
		rtn=1;
	}
	fclose(fp);
	return rtn;
}

int Client_Message_Save(char *name,message_node_t *data)
{
    char path[50]="./";
    int rtn=0;
	FILE *fp;
    strcat(path,name);
    strcat(path,"/Private/Chat.dat");
	fp=fopen(path,"ab");
	if(fp==NULL)
	{
	    printf("File Open Fail !\n");
    }
	else
	{
		fwrite(data,sizeof(message_node_t),1,fp);
		rtn=1;
	}
	fclose(fp);
	return rtn;
}

int Client_Group_Message_Save(char *name,message_node_t *data)  //用户信息写入函数,返回1 表示操作成功，0 表示操作失败
{
    int rtn=0;
    char file[50]="./";
	FILE *fp;
    strcat(file,name);
    strcat(file,"/Group/Chat.dat");
	fp=fopen(file,"ab");
	if(fp==NULL)
	{
		printf("File Open Fail !\n");
    }
	else
	{
		fwrite(data,sizeof(message_node_t),1,fp);
		rtn=1;
	}
	fclose(fp);
	return rtn;
}


int UserInfo_SelectByName(char *name) //通过用户名查找该用户是否存在,1表示存在。0表示不存在
{
    int found=0;
    message_node_t buf;
    FILE *fp;
    fp=fopen(USERINFO_DATA_FILE,"rb");
    if(fp==NULL)
    {
        return 0;
    }
    else
    {
        while(fread(&buf,sizeof(message_node_t),1,fp)==1)
        {
        	if(!strcmp(name,buf.Sendname))
            {
                found=1;
                break;
            }
        }
    }
    fclose(fp);
    return found;
}

int UserInfo_Perst_Select(char *name,message_node_t *buf)  //通过用户名，找到用户相关信息返回1表示找到，0表示未找到 
{
	int found=0;
    FILE *fp;
    fp=fopen(USERINFO_DATA_FILE,"rb");
    if(fp==NULL)
    {
        printf("文件打开失败！");
    }
    else
    {
        while(fread(buf,sizeof(message_node_t),1,fp)==1)
        {
        	if(!strcmp(name,buf->Sendname))
            {
                found=1;
                break;
            }
        }
    }
    fclose(fp);
    return found;
}

//将参数所指向的新信息写入到文件中，返回0表示操作失败，返回1表示操作成功
int Play_Perst_Update(const message_node_t *data) 
{
	FILE *fp = fopen(USERINFO_DATA_FILE, "rb+");
	if (NULL == fp)
	{
		printf("打开文件失败！");
		return 0;
	}
	message_node_t buf;
	int found = 0;
	while (!feof(fp)) 
	{
		if (fread(&buf, sizeof(message_node_t), 1, fp)) 
		{
			if(!strcmp(buf.Sendname,data->Sendname)) 
			{
				fseek(fp, -sizeof(message_node_t), SEEK_CUR);
				fwrite(data, sizeof(message_node_t), 1, fp);
				found = 1;
				break;
			}
		}
	}
	fclose(fp);
	return found;
}


int Private_Message_SelectAll(char *name,message_node_t *list) 
{
	int found=0;
	FILE *fp;
    char file[50]="./";
    strcat(file,name);
    strcat(file,"/Private/Chat.dat");
	fp=fopen(file,"rb");
	if(fp==NULL)
	{
		printf("文件打开失败！\n");
		return 0; 
	}
	else
	{
		List_Init(list, message_node_t);
		message_node_t buf;
		message_node_t *p;
		p=(message_node_t *)malloc(sizeof(message_node_t));
		while(fread(&buf,sizeof(message_node_t),1,fp)==1)
		{
            p->flag=buf.flag;
			p->Sendtime=buf.Sendtime;
            strcpy(p->Sendname,buf.Sendname);
            strcpy(p->Recvname,buf.Recvname);
            strcpy(p->Message,buf.Message);
            List_AddHead(list, p);
			p=(message_node_t *)malloc(sizeof(message_node_t));
			found++;
		}
	}
	fclose(fp); 
	return found;
}


int Group_Message_SelectAll(char *name,message_node_t *list) 
{
	int found=0;
	FILE *fp;
    char file[50]="./";
    strcat(file,name);
    strcat(file,"/Group/Chat.dat");
	fp=fopen(file,"rb");
	if(fp==NULL)
	{
		printf("文件打开失败！\n");
		return 0; 
	}
	else
	{
		List_Init(list, message_node_t);
		message_node_t buf;
		message_node_t *p;
		p=(message_node_t *)malloc(sizeof(message_node_t));
		while(fread(&buf,sizeof(message_node_t),1,fp)==1)
		{
            p->flag=buf.flag;
			p->Sendtime=buf.Sendtime;
            strcpy(p->Sendname,buf.Sendname);
            strcpy(p->Recvname,buf.Recvname);
            strcpy(p->Message,buf.Message);
            List_AddHead(list, p);
			p=(message_node_t *)malloc(sizeof(message_node_t));
			found++;
		}
	}
	fclose(fp); 
	return found;
}

#endif
