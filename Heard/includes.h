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

//���Ϊ���ԣ���Ϊ1
#define SYS_DEBUG	0


//������ͨѶ
#define ADD_SOFT_COMM 			1

#define SERV_IP 				"127.0.0.1"
#define SERV_PORT 				7815
#define SOFT_SERV_PORT 			7818

#define SOCKET_MAX_NUM			60000

#define SOFT_SOCKET_MAX_NUM		60000

//�豸������������
#define DEV_BUFFER_POOL_MAX_VAL 3
//�豸������󳤶�
#define CMD_MAX_LEN	256
//���������������
#define SOFT_BUFFER_POOL_MAX_VAL 3


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


//�������ڵ�
typedef struct _BUFFER_POOL2_NODE
{
	char cmd[CMD_MAX_LEN];//����
	int cmdLen;//�����
	int sockfd;//socket�ļ�������
	int useFlag;//ʹ�ñ�־
}BUFFER_POOL2_NODE;



//��Ҫ���ڴ��С2+2+(65535*(DEV_CMD_MAX_LEN+4+4)) = 17301244 = 16.4MB
typedef struct _BUFFER_POOL2
{
	unsigned short int readIndex;
	unsigned short int writeIndex;
	BUFFER_POOL2_NODE bpNode[65535];	
}BUFFER_POOL2;


/*********************************/
//  brks ��Э��
/*typedef struct brks_pro{
	unsigned char head[4];//--�ַ�hhhh
	unsigned char ver;//--�汾��
	unsigned char proType;//--0:ʮ�����ƣ�1:����
	unsigned short msg_id;//--��Ϣid
	unsigned int dataLen;//--���ݳ���
	void * pbuf;//--��dataLenΪ0ʱ������û����
	unsigned char end[4];//--�ַ�eeee
}Brks_Pro;


//proTypeΪ0ʱ��void *bufΪ���½ṹ 
typedef struct VOID_BUF_0
{
	unsigned char devType;//--10����11�ƶ��豸
	unsigned char devID[16];//�豸��16λID
	unsigned char action;//--��Ҫִ�еĶ���,0Ϊִ�гɹ���Ӧ��
	unsigned short dataLen;//--�������ݵĳ���
	unsigned char[2] datas;//--���dataLenΪ0����û��
}VoidBuf0;*/

// brksЭ�����
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

