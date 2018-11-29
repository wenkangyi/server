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
#include <signal.h>
#include <sys/resource.h>


#include "mysql/mysql.h"


#include <pthread.h>
#include "string.h"
#include "unistd.h"
#include "mysqltest.h"
#include "FileLog.h"
#include "externFun.h"

//���Ϊ���ԣ���Ϊ1
#define SYS_DEBUG	1


//������ͨѶ
#define ADD_SOFT_COMM 			1

#define SERV_IP 				"127.0.0.1"
#define SERV_PORT 				7815
#define SOFT_SERV_PORT 			7818

#define SOCKET_MAX_NUM			60000

#define SOFT_SOCKET_MAX_NUM		60000

//�豸ID��󳤶�
#define DEVICE_ID_MAX_LEN 20

//��ʱ������
#define TIMEROUT_TICKS	200

#define MAXLINE			1024

#define ID_16_BIT

#ifndef ID_16_BIT//���û�ж���16λ��������24λ
	#define ID_24_BIT
#endif


#if defined(ID_16_BIT)
	#define ID_LEN	17//���һ��Ϊ������
#elif defined(ID_24_BIT)
	#define ID_LEN	25
#endif

//�����ģʽ���Զ��ֹ���
//Э��͸��ģʽ
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
	//socket�����ļ�������
	Int32 _socketFD;
	//�����߳�ID
	pthread_t _receiveThreadID;
	//ͨѶ��ʱ���
	uInt16 _timerout;
	//�߳�״̬
	THREAD_STATUS _threadStatus;
	//�豸ID��������ʱ����Ҫ����ID
	//char _deviceID[DEVICE_ID_MAX_LEN];
	//ָ����һ�ڵ�
	struct _SOCKET_PARAM *nextlink;
}SOCKET_PARAM;

//�豸�ڵ�ID����������ӽṹ���У��ɶ�̬�����ڴ�õ�
typedef struct _DEVICE_ID{
	char _deviceID[DEVICE_ID_MAX_LEN];
	struct _DEVICE_ID *nextlink;
}DEVICE_ID;

//������Ӵ���ṹ��
typedef struct _SOFT_PARAM{
	//����������ļ�������
	Int32 _socketFD;
	//�����߳�ID
	pthread_t _receiveThreadID;
	//ͨѶʱ�䳬ʱ
	uInt16 _timerout;
	//�߳�״̬
	THREAD_STATUS _threadStatus;
	//����ɶ�̬�����ڴ�õ�
	DEVICE_ID *pDeviceID;
	//ָ����һ�ڵ�
	struct _SOFT_PARAM *nextlink;
}SOFT_PARAM;

//������нṹ��
typedef struct _BUFFER_POOL
{
	char *cmd;
	int cmdLen;
	int sockfd;
	struct _BUFFER_POOL *nextlink;
}BUFFER_POOL;


extern void *
AcceptThread(void *arg);
#if ADD_SOFT_COMM
extern void *
AcceptSoftThread(void *arg);
#endif
extern void SystemParamInit(void);



#endif

