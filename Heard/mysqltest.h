#ifndef __MYSQL_TEST_H__
#define __MYSQL_TEST_H__

#include "includes.h"


extern int init_mysql(MYSQL *g_conn);
extern int init_mysql_conn(MYSQL *g_conn);
extern void CreateTable(MYSQL *g_conn);
extern void init_tables(MYSQL *g_conn);
extern int executesql(MYSQL *g_conn,const char * sql);
extern unsigned char InsertID(MYSQL *g_conn,char *idstr);
extern int CheckUserName(MYSQL *g_conn,char *buf,int softSocketFD);
extern int JudgeIDToDeviceLink(MYSQL *g_conn,char *pID,int socketID);
extern int GetSoftSocketID(MYSQL *g_conn,char *pID,int socketIDFlag);
extern int InDeviceSockFDOutSoftSockFD(MYSQL *g_conn,int deviceSockFD);
extern int RecordSocketID(MYSQL *g_conn,char *buf,int socketID);
extern int ClearSoftSocketFD(MYSQL *g_conn,int softSocketID);
extern int ClearDeviceSocketFD(MYSQL *g_conn,int deviceSocketID);
extern int JudgeSocketFDUserName(MYSQL *g_conn,int softSocketID);

#endif

