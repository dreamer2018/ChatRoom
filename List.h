/*************************************************************************
	> File Name: List.h
	> Author: ZhouPan / git:dreamer2018
	> Mail: zhoupans_mail@163.com
	> Blog: blog.csdn.net/it_dream_er 
	> Created Time: Sat 08 Aug 2015 04:12:13 PM CST
 ************************************************************************/

#ifndef _LIST_H
#define _LIST_H

#include<assert.h>
#define BUFSIZE 1024
//交互信息结构体

typedef struct Message
{
    int flag;               //0 系统命令，1 用户注册，2 用户登录，3 用户群聊,4 用户私聊
    char Sendname[21];      // 发送者，系统为system 
    char Recvname[21];      // 接收者，系统为system ，所有人为everyone 
    char Message[BUFSIZE];      // 消息内容
    time_t Sendtime;        // 发送时间
    struct Message *head;
    struct Message *prev;
}message_node_t;

/*初始化链表list,链表为带头结点的双向循环链表*/

#define List_Init(list, list_node_t) {					\
		list=(list_node_t*)malloc(sizeof(list_node_t)); \
		(list)->next=(list)->prev=list;					\
}

//释放链表list中所有数据结点。list 为链表头指针，list_node_t为链表结点类型

#define List_Free(list, list_node_t) {			\
		assert(NULL!=list);						\
		list_node_t *tmpPtr;					\
		(list)->prev->next=NULL; 				\
		while(NULL!=(tmpPtr=(list)->next)){ 	\
			(list)->next=tmpPtr->next;			\
			free(tmpPtr);						\
		}										\
		(list)->next=(list)->prev=list;			\
}

//销毁链表list，释放所有数据结点及头结点。 list为链表头指针，tmpPtr为链表结点临时指针变量

#define List_Destroy(list, list_node_t) {		\
		assert(NULL!=list);						\
		List_Free(list, list_node_t)			\
		free(list);								\
		(list)=NULL;							\
}

//链表头插法，list为头指针，new为新节点

#define List_AddHead(list, newNode) {			\
		(newNode)->next=(list)->next;		 	\
		(list)->next->prev=newNode;			 	\
		(newNode)->prev=(list);				 	\
		(list)->next=newNode;				 	\
	}

//链表尾插法，list为头指针，new为新节点

#define List_AddTail(list, newNode) {			\
		(newNode)->prev=(list)->prev; 		 	\
		(list)->prev->next=newNode;			 	\
		(newNode)->next=list;				 	\
		(list)->prev=newNode;				 	\
}


//从删除链表结点node，

#define List_DelNode(node) { \
			assert(NULL!=node && node!=(node)->next && node!=(node)->prev);				\
			(node)->prev->next=(node)->next; 	\
			(node)->next->prev=(node)->prev;	\
}

//从链表中删除并释放结点node

#define List_FreeNode(node) {	\
		List_DelNode(node);		\
		free(node);				\
}

#endif
