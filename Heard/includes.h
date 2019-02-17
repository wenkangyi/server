#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <netinet/in.h>
#include <sys/epoll.h>
#include <errno.h>

#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <memory.h>
#include <signal.h>
#include <sys/resource.h>


#include "mysql/mysql.h"


#include <pthread.h>
#include "string.h"
#include "unistd.h"
#include "mysqltest.h"
#include "FileLog.h"
#include "externFun.h"

//如果为调试，即为1
#define SYS_DEBUG	0


//添加软件通讯
#define ADD_SOFT_COMM 			1

#define SERV_IP 				"127.0.0.1"
#define SERV_PORT 				7815
#define SOFT_SERV_PORT 			7818

#define SOCKET_MAX_NUM			60000

#define SOFT_SOCKET_MAX_NUM		60000

//设备缓冲池最大组数
#define DEV_BUFFER_POOL_MAX_VAL 3
//设备命令最大长度
#define CMD_MAX_LEN	256
//软件缓冲池最大组数
#define SOFT_BUFFER_POOL_MAX_VAL 3


//设备ID最大长度
#define DEVICE_ID_MAX_LEN 20

//超时节拍数
#define TIMEROUT_TICKS	200

#define MAXLINE			1024

#define ID_16_BIT

#ifndef ID_16_BIT//如果没有定义16位，即定义24位
	#define ID_24_BIT
#endif


#if defined(ID_16_BIT)
	#define ID_LEN	17//最后一个为结束符
#elif defined(ID_24_BIT)
	#define ID_LEN	25
#endif

//下面的模式可以多种共存
//协议透传模式
#ifndef AGREEMENT_PASSTHROUGH_MODE
#define AGREEMENT_PASSTHROUGH_MODE
#endif


#ifndef _SOFT_SOCKET_ID
#define _SOFT_SOCKET_ID 	0
#endif


#ifndef _DEVICE_SOCKET_ID
#define _DEVICE_SOCKET_ID 	1
#endif

#ifndef USER_NAME_MAX_LEN
#define USER_NAME_MAX_LEN	21
#endif

#ifndef USER_PASSWORD_MAX_LEN
#define USER_PASSWORD_MAX_LEN	19
#endif

typedef char Int8 ;
typedef unsigned char uInt8;

typedef short int Int16;
typedef unsigned short int uInt16;

typedef int Int32;
typedef unsigned int uInt32;

typedef long Int64;
typedef unsigned long uInt64;

typedef enum{
	_threadNULL=0,
	_threadRUN,
	_threadEXIT,
	_threadEND
}THREAD_STATUS;

//
typedef struct _SOCKET_PARAM {
	//socket连接文件描述符
	Int32 _socketFD;
	//接收线程ID
	pthread_t _receiveThreadID;
	//通讯超时检测
	uInt16 _timerout;
	//线程状态
	THREAD_STATUS _threadStatus;
	//设备ID，连接上时，需要先问ID
	//char _deviceID[DEVICE_ID_MAX_LEN];
	//指向下一节点
	struct _SOCKET_PARAM *nextlink;
}SOCKET_PARAM;

//设备节点ID，在软件连接结构体中，由动态申请内存得到
typedef struct _DEVICE_ID{
	char _deviceID[DEVICE_ID_MAX_LEN];
	struct _DEVICE_ID *nextlink;
}DEVICE_ID;

//软件连接处理结构体
typedef struct _SOFT_PARAM{
	//软件的连接文件描述符
	Int32 _socketFD;
	//接收线程ID
	pthread_t _receiveThreadID;
	//通讯时间超时
	uInt16 _timerout;
	//线程状态
	THREAD_STATUS _threadStatus;
	//这个由动态申请内存得到
	DEVICE_ID *pDeviceID;
	//指向下一节点
	struct _SOFT_PARAM *nextlink;
}SOFT_PARAM;

//缓冲队列结构体
typedef struct _BUFFER_POOL
{
	char *cmd;
	int cmdLen;
	int sockfd;
	struct _BUFFER_POOL *nextlink;
}BUFFER_POOL;


//缓冲区节点
typedef struct _BUFFER_POOL2_NODE
{
	char cmd[CMD_MAX_LEN];//命令
	int cmdLen;//命令长度
	int sockfd;//socket文件描述符
	int useFlag;//使用标志
}BUFFER_POOL2_NODE;



//需要的内存大小2+2+(65535*(DEV_CMD_MAX_LEN+4+4)) = 17301244 = 16.4MB
typedef struct _BUFFER_POOL2
{
	unsigned short int readIndex;
	unsigned short int writeIndex;
	BUFFER_POOL2_NODE bpNode[65535];	
}BUFFER_POOL2;


/*********************************/
//  brks 的协议
/*typedef struct brks_pro{
	unsigned char head[4];//--字符hhhh
	unsigned char ver;//--版本号
	unsigned char proType;//--0:十六进制，1:其它
	unsigned short msg_id;//--消息id
	unsigned int dataLen;//--数据长度
	void * pbuf;//--当dataLen为0时，这里没数据
	unsigned char end[4];//--字符eeee
}Brks_Pro;


//proType为0时，void *buf为以下结构 
typedef struct VOID_BUF_0
{
	unsigned char devType;//--10车，11移动设备
	unsigned char devID[16];//设备的16位ID
	unsigned char action;//--需要执行的动作,0为执行成功的应答
	unsigned short dataLen;//--后面数据的长度
	unsigned char[2] datas;//--如果dataLen为0，即没有
}VoidBuf0;*/

// brks协议结束
/*********************************/






extern void *
AcceptThread(void *arg);
#if ADD_SOFT_COMM
extern void *
AcceptSoftThread(void *arg);
#endif
extern void SystemParamInit(void);

extern void InitBufferPool2();

#endif

