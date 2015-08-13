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
int User_Init(char *user_name)
{
    char path[30]="./user/";
    char command1[40]="mkdir ";
    char command2[40]="touch ";
    strcat(path,user_name);
    if(!Exists(path))
    {
        strcat(command1,path);
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

/*
int Play_Perst_SelectAll(play_list_t list) 
{
	int found=0;
	FILE *fp;
	fp=fopen(PLAY_DATA_FILE,"rb");
	if(fp==NULL)
	{
		printf("文件打开失败！\n");
		return 0; 
	}
	else
	{
		List_Free(list, play_node_t);
		play_t buf;
		play_node_t *p;
		p=(play_node_t *)malloc(sizeof(play_node_t));
		while(fread(&buf,sizeof(play_t),1,fp)==1)
		{
			p->data.id=buf.id;
			strcpy(p->data.name,buf.name);
			p->data.type=buf.type;
			strcpy(p->data.area,buf.area);
			p->data.rating=buf.rating;
			p->data.duration=buf.duration;
			p->data.start_date=buf.start_date;
			p->data.end_date=buf.end_date;
			p->data.price=buf.price;
			List_AddTail(list, p);
			p=(play_node_t *)malloc(sizeof(play_node_t));
			found++;
		}
	}
	fclose(fp); 
	return found;
}
*/















#endif
