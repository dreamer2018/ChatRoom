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
    struct Message *next;
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
//分页数据结构体，简称分页器类型
typedef struct
{
	int totalRecords;	//总记录数
	int offset;			//当前页起始记录相对于第一条记录的偏移记录数
	int pageSize;		//页面大小
	void *curPos;		//当前页起始记录在链表中的结点地址
}Pagination_t;

//根据分页器paging的偏移量offset将分页器定位到链表list的对应位置
#define List_Paging(list, paging, list_node_t) {			\
		if(paging.offset+paging.pageSize>=paging.totalRecords){	\
			Paging_Locate_LastPage(list, paging, list_node_t);	}\
		else {													\
			int i;												\
			list_node_t * pos=(list)->next;						\
			for( i=0; i<paging.offset && pos!=list ; i++) 		\
			   pos=pos->next;		 							\
			paging.curPos=(void*)pos;							\
		}														\
	}

//将分页器paging定位到链表list的第一页
#define Paging_Locate_FirstPage(list, paging) { \
		paging.offset=0;						\
		paging.curPos=(void *)((list)->next);	\
	}

//将分页器paging定位到链表list的最后一页
#define Paging_Locate_LastPage(list, paging, list_node_t) {	\
	int i=paging.totalRecords % paging.pageSize;	\
	if (0==i && paging.totalRecords>0)				\
		i=paging.pageSize;							\
	paging.offset=paging.totalRecords-i;			\
	list_node_t * pos=(list)->prev;					\
	for(;i>1;i--)									\
		pos=pos->prev;								\
	paging.curPos=(void*)pos;						\
													\
}

//对于链表list及分页器paging,使用指针curPos依次遍历paging指向页面中每个结点
//这里i为整型计数器变量
#define Paging_ViewPage_ForEach(list, paging, list_node_t, pos, i) 	\
	for (i=0, pos = (list_node_t *) (paging.curPos);	\
			pos != list && i < paging.pageSize; 		\
			i++, pos=pos->next)							\


//对于链表list,将分页器paging向前（后）移动offsetPage个页面.
//当offsetPage<0时，向前（链表头方向）移动|offsetPage|个页面
//当offsetPage>0时，向后（链末尾方向）移动offsetPage个页面
#define Paging_Locate_OffsetPage(list, paging, offsetPage, list_node_t) {\
	int offset=offsetPage*paging.pageSize;			\
	list_node_t *pos=(list_node_t *)paging.curPos;	\
	int i;											\
	if(offset>0){									\
		if( paging.offset + offset >= paging.totalRecords )	{\
			Paging_Locate_LastPage(list, paging, list_node_t);	\
		}else {												\
			for(i=0; i<offset; i++ )						\
				pos=pos->next;								\
			paging.offset += offset;						\
			paging.curPos= (void *)pos;						\
		}													\
	}else{													\
		if( paging.offset + offset <= 0 ){					\
			Paging_Locate_FirstPage(list, paging);			\
		}else {												\
			for(i=offset; i<0; i++ )						\
				pos	= pos->prev;							\
			paging.offset += offset;						\
			paging.curPos= pos;								\
		}													\
	}														\
}

//根据分页器paging计算当前的页号
#define Pageing_CurPage(paging) 	(0==(paging).totalRecords?0:1+(paging).offset/(paging).pageSize)

//根据分页器paging计算的总的页数
#define Pageing_TotalPages(paging) 	(((paging).totalRecords%(paging).pageSize==0)?\
	(paging).totalRecords/(paging).pageSize:\
	(paging).totalRecords/(paging).pageSize+1)

//根据paging判断当前页面是否为第一页。结果为true表示是，否则false
#define Pageing_IsFirstPage(paging) (Pageing_CurPage(paging)<=1)

//根据paging判断当前页面是否为最后一页。结果为true表示是，否则false
#define Pageing_IsLastPage(paging) 	(Pageing_CurPage(paging)>=Pageing_TotalPages(paging))

#endif
