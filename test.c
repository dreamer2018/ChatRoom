/*************************************************************************
	> File Name: test.c
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Tue 11 Aug 2015 04:17:37 PM CST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
int main()
{
    char temp[1024];
    char name[21];
    char message[512];
    gets(temp);
    
    if(strlen(temp)>512)
    {
        printf("send fail,message too long\n");
    }
    if(!strncmp(temp,"@",1))
    {
        int i,j=-1,k=-1;
        for(i=1;j<20;i++)
        {
            if(temp[i]==' ')
                break;
            j++;
            name[j]=temp[i];
        }
        name[j]='\0';
        for(j=i;j<512;j++)
        {
            if(temp[j]==' ')
                continue;
            while(1)
            {
                k++;
                message[k]=temp[j];
                j++;
                if(temp[j]=='\0')
                    break;
            }
            break;
        }
    }
    else if(!strncmp(temp,"//",2))
    {
        int i,j=-1,k=-1;
        for(i=2;j<20;i++)
        {
            if(temp[i]==' ')
                break;
            j++;
            name[j]=temp[i];
        }
        name[j]='\0';
        printf("i=%d\n",i);
        for(j=i;j<512;j++)
        {
            if(temp[j]==' ')
                continue;
            while(1)
            {
                k++;
                message[k]=temp[j];
                j++;
                if(temp[j]=='\0')
                    break;
            }
            break;
        }
    }
    else
    {
        strcpy(name,"my");
        strcpy(message,temp);
    }
    printf("name:%s\n",name);
    printf("message:%s\n",message);
}
