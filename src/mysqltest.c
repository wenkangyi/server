/********************************************************************
* ���⣺C��������mysql���ݿ⣬ʵ�ֿ�����ɾ�Ĳ�Ľ�ɫȨ�޵�¼ϵͳ
* 
* ��������������ڰ�װGCC���뻷����mysql���ɻ�����ֱ�ӱ������У����ݲ�ͬʹ���ߵ�mysql�˻����޸Ĵ���ǰ��������Գɹ�����mysql���ݿ�
* �������gcc mysqltest.c -lmysqlclient -o mysqltest
********************************************************************/


#include "includes.h"

//MYSQL *g_conn;//mysql ����
//MYSQL_RES *g_res;//mysql ��¼��
//MYSQL_ROW g_row;//�ַ������飬mysql ��¼��

#define MAX_BUF_SIZE 2048 //����������ֽ���
/*=================================================================*/
/**/const char *g_host_name = "localhost";
/**/const char *g_user_name = "root";
/**/const char *g_password = "0136825";
/**/const char *g_db_name = "mysql";
/**/const char *g_db_usr_name = "iotDataBase";
/**/const unsigned int g_db_port = 3306;
/*=================================================================*/
char sql[MAX_BUF_SIZE];
char Time[MAX_BUF_SIZE];

int iNum_rows = 0;//mysql���ִ�н��������������ֵ
int flag = 0;//����ԱȨ�޿���
int i = 1;//ϵͳ���п���
pthread_mutex_t mutexMySQL=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexMySQLConn=PTHREAD_MUTEX_INITIALIZER;



//��¼ʹ�õĽṹ��
struct Login
{
	char name[24];
	char password[20];
	
	//_name varchar(24) not null unique,
	//_password char(20) not null,
	//_create_time datetime not null,
	//_permissions int,
	//_remark varchar(200)
	
}login;
//����ʹ�õĽṹ��
struct Operation
{
	char tables[24];
	char name[24];
	char passwd[20];
	char role[24];
	char remark[20];
}ope;

//��������
void create_database(MYSQL *g_conn);
void CheckAdminUser(MYSQL *g_conn);




void ClearSqlStr(void)
{
	int i = 0;
	
	for(i=0;i<MAX_BUF_SIZE;i++)
		sql[i] = 0;
}

//��ӡ������Ϣ
void print_mysql_error(MYSQL *g_conn,const char *msg)
{
	if(msg)
		printf("%s: %s\n",msg,mysql_error(g_conn));
	else
		puts(mysql_error(g_conn));
}
//ִ��sql��䣬�ɹ�����0��ʧ�ܷ���-1
int executesql(MYSQL *g_conn,const char * sql)
{
	char msg[1024];
	memset(msg,0,1024);
	sprintf(msg,"g_conn = 0x%x,sql = %s",g_conn,sql);
	WriteLog(msg);
	int result =0;
	pthread_mutex_lock(&mutexMySQL);
	if(mysql_real_query(g_conn,sql,strlen(sql)))
	{
		result = -1;	
	}
	pthread_mutex_unlock(&mutexMySQL);	
	return result;
}

int init_mysql_conn(MYSQL *g_conn)
{
	int result = 0;
	//mutexMySQLConn
	pthread_mutex_lock(&mutexMySQLConn);
	if(!mysql_real_connect(g_conn,g_host_name,g_user_name,g_password,g_db_name,g_db_port,NULL,0))
		result = -1;//����ʧ��
	pthread_mutex_unlock(&mutexMySQLConn);
	//����Ƿ����ʹ��
	if(executesql(g_conn,"set names utf8"))
		result = -1;

	return result;
}

//��ʼ������
int init_mysql(MYSQL *g_conn)
{		
	//connection the database
	if(!mysql_real_connect(g_conn,g_host_name,g_user_name,g_password,g_db_name,g_db_port,NULL,0))
		return -1;//����ʧ��
	//����Ƿ����ʹ��
	if(executesql(g_conn,"set names utf8"))
		return -1;
	//
	#if SYS_DEBUG
	printf("create_database \r\n");
	#endif
	create_database(g_conn);
	#if SYS_DEBUG
	printf("CreateTable \r\n");
	#endif
	CreateTable(g_conn);
	#if SYS_DEBUG
	printf("CheckAdminUser \r\n");
	#endif
	CheckAdminUser(g_conn);
	#if SYS_DEBUG
	printf("mysql_init g_conn = %ld \r\n",(long int)g_conn);
	#endif
	return 0;//���سɹ�
}

//select COUNT(1) as count from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='���ݿ���' and TABLE_NAME='�����';
//�����
uInt8 CheckTable(MYSQL *g_conn,char *tableName)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	int i = 0;
	uInt8 flag = 0;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select COUNT(1) as count from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='");//�����ַ���ƴ��
	strcat(sqlstr,g_db_usr_name);
	strcat(sqlstr,"' and TABLE_NAME='");
	strcat(sqlstr,tableName);
	strcat(sqlstr,"';");
	
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows > 0)
	{
		_g_row=mysql_fetch_row(_g_res);						
		flag = atoi(_g_row[0]);//��ȡ����
	}
	else
	{
		flag = 0;
	}
	mysql_free_result(_g_res); // �ͷŽ����

	return flag;
}

void CreateTable(MYSQL *g_conn)
{
	int flag = 0;
	if(CheckTable(g_conn,"users") == 0)
	{
		//_id--
		//_name--�û���
		//_password--�û���¼����
		//_create_time--�û�����ʱ��
		//_permissions--�û�Ȩ��
		//_remark--��ע
		flag = executesql(g_conn,"create table users(_id smallint unsigned primary key auto_increment,_name varchar(24) not null unique,_password char(20) not null,_create_time datetime,_permissions int,_softSocketID int default -1 not null,__remark varchar(200));");
	}
	//���豸���Ӽ�����ת���й�
	if(CheckTable(g_conn,"deviceLink") == 0)
	{
		//_id
		//_deivceID--�豸��ID
		//_deviceSocketID--�豸��socketID
		//_softSocketID--�ͻ��������socketID
		//_name--���豸���ĸ��û���ע��
		//_caption--0Ϊ�·�����ID�����ã�1Ϊ���õ�ID
		flag = executesql(g_conn,"create table deviceLink(_id smallint unsigned primary key auto_increment,_deviceID varchar(30) not null unique,_create_id_time datetime,_deviceSocketID int default -1 not null,_softSocketID int default -1 not null,_name varchar(200),_use_id_time datetime,_caption int);");
	}
}

//��ʼ����������socketFD��Ҫ��ʼ��Ϊ-1
void init_tables(MYSQL *g_conn)
{
	executesql(g_conn,"update users set _softSocketID=-1;");
	executesql(g_conn,"update deviceLink set _deviceSocketID=-1,_softSocketID=-1;");
}

//���ݵõ���ID�������ݽ����ж�

int JudgeIDToDeviceLink(MYSQL *g_conn,char *pID,int socketID)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select * from deviceLink where _deviceID='%s';",pID);
	WriteLog(sqlstr);
	
	#if SYS_DEBUG
	printf("g_conn:%ld \r\n",(long int)g_conn);
	#endif
	executesql(g_conn,sqlstr);	
	_g_res = mysql_store_result(g_conn);
	#if SYS_DEBUG
	printf("_g_res:%ld \r\n",(long int)_g_res);
	#endif
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0) return 0;
	sprintf(sqlstr,"update deviceLink set _deviceSocketID=%d where _deviceID='%s';",socketID,pID);
	WriteLog(sqlstr);
	
	int rt = executesql(g_conn,sqlstr);
	
	if(rt == 0)
		return 1;
	return 0;
}

//����deviceID��ȡsoftSocketID
int GetSoftSocketID(MYSQL *g_conn,char *pID,int socketIDFlag)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	int i = 0;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select _softSocketID,_deviceSocketID from deviceLink where _deviceID='%s' and _deviceSocketID!=-1;",pID);
	WriteLog(sqlstr);
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	if(_g_res == 0x00) return -1;
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows > 0){
		int iNum_fields = mysql_num_fields(_g_res); // �õ���¼������
		_g_row=mysql_fetch_row(_g_res);
		int socketID = atoi((const char *)_g_row[socketIDFlag]);

		return socketID;
	}

	return -1;
}

//����Ӳ����socket_fd��ȡ�����socket_fd
int InDeviceSockFDOutSoftSockFD(MYSQL *g_conn,int deviceSockFD)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	int i = 0;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select _softSocketID from deviceLink where _deviceSocketID='%d' and _softSocketID is not null;",deviceSockFD);
	WriteLog(sqlstr);
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	if(_g_res == 0x00) return -1;
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows > 0){
		int iNum_fields = mysql_num_fields(_g_res); // �õ���¼������
		_g_row=mysql_fetch_row(_g_res);
		int socketID = atoi((const char *)_g_row[0]);

		return socketID;
	}

	return -1;

}

//�˺�������û����������Ƿ���ȷ
//����1����Ϊ�û������󣬼������ڴ��û�����
//����2����Ϊ�û��������
//����3����Ϊ�û������ȳ���20�ַ�
//����4����Ϊ�û����볤�ȳ���18�ַ�
//����0������֤��ȷ
int CheckUserName(MYSQL *g_conn,char *buf,int softSocketFD)
{
	char userName[USER_NAME_MAX_LEN];
	char userPassword[USER_PASSWORD_MAX_LEN];
	int i = 0;
	int index = 0;
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	
	for(i=0;i<USER_NAME_MAX_LEN;i++) userName[i] = 0;
	for(i=0;i<USER_PASSWORD_MAX_LEN;i++) userPassword[i] = 0;
	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	i = 0;
	while((buf[index] != ',') 
		&& (i<(USER_NAME_MAX_LEN - 1))){
		userName[i++] = buf[index++];
	}
	WriteLog(userName);
	if (buf[index] != ',') return 3;//�û������ȴ���
	
	index ++;
	i = 0;
	while((buf[index] != '\r') 
		&& (i<(USER_PASSWORD_MAX_LEN- 1))){
		userPassword[i++] = buf[index++];
	}
	WriteLog(userPassword);
	if (buf[index] != '\r') return 4;//���볤�ȴ���
	
	sprintf(sqlstr,"select _password from users where _name='%s';",userName);
	WriteLog(sqlstr);
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0) return 1;//�����ڴ��û���

	_g_row=mysql_fetch_row(_g_res);
	
	if(strcmp(_g_row[0],userPassword) == 0){
		//��softSocketIDд�����ݿ�
		sprintf(sqlstr,"update users set _softSocketID=%d where _name='%s';",softSocketFD,userName);
		executesql(g_conn,sqlstr);
		return 0;//��֤��ȷ
	}
	else
		return 2;//�������
}

//����softSocketID�����ݿ��в��Ҽ�¼��
//���û�����¼ʱ����Ҫ���˲���д�����ݿ�
//����0���޼�¼������1���м�¼
int JudgeSocketFDUserName(MYSQL *g_conn,int softSocketID)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select * from users where _softSocketID='%d';",softSocketID);
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0) return 0;//û��¼
	
	return 1;//�м�¼	
}

//���³ɹ�����0�����ɹ�������1
int ClearSoftSocketFD(MYSQL *g_conn,int softSocketID)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	for(i=0;i<1024;i++) sqlstr[i] = 0;

	sprintf(sqlstr,"update deviceLink set _softSocketID=-1 where _softSocketID='%d';",softSocketID);
	WriteLog(sqlstr);
	int rt = executesql(g_conn,sqlstr);
	if( rt == 0)
		return 0;
	return 1;
}

//���³ɹ�����0�����ɹ�������1
int ClearDeviceSocketFD(MYSQL *g_conn,int deviceSocketID)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	for(i=0;i<1024;i++) sqlstr[i] = 0;

	sprintf(sqlstr,"update deviceLink set _deviceSocketID=-1 where _deviceSocketID='%d';",deviceSocketID);
	WriteLog(sqlstr);
	int rt = executesql(g_conn,sqlstr);
	if( rt == 0)
		return 0;
	return 1;
}


//�ϴ���ID��ȷ��������0��ID���󣬼�����1
//���ID���Ȳ�Ϊ 16��������2
int RecordSocketID(MYSQL *g_conn,char *buf,int socketID)
{
	char deviceID[ID_LEN];
	int i = 0;
	int index = 0;
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	
	for(i=0;i<ID_LEN;i++) deviceID[i] = 0;
	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	if(StringSub(buf,deviceID,ID_LEN) == 0) return 2;
	
	sprintf(sqlstr,"select * from deviceLink where _deviceID='%s';",deviceID);

	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0) return 1;//�����ڴ�ID

	sprintf(sqlstr,"update deviceLink set _softSocketID=%d where _deviceID='%s';",socketID,deviceID);
	WriteLog(sqlstr);
	int rt = executesql(g_conn,sqlstr);
	
	if( rt == 0)
		return 0;
	return 1;
}

//����Ƿ����Admin�û��������ڣ����Զ�����һ��
void CheckAdminUser(MYSQL *g_conn)
{
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	//sprintf(sql,"select * from users where _name=Admin");
	executesql(g_conn,"select * from users where _name='Admin';");
	_g_res = mysql_store_result(g_conn);
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0)
	{
		//���Admin�û�
		//sprintf(sql,"insert into users values(%d,'%s','%s','%s',%s,'%s');",i,ope.name,ope.passwd,Time,ID,ope.remark);
  		executesql(g_conn,"INSERT INTO users(_name, _password,_permissions) VALUES('Admin', 'Wen0136825',0);");
	}
}

//ID���ڣ�������1�������ڣ�������0
uInt8 CheckID(MYSQL *g_conn,char *idstr)
{
	char sqlstr[1024];
	int i = 0;
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;

	for (i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select * from deviceLink where _deviceID='%s';",idstr);
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0)
		return 0;
	else
		return 1;
}

//�ɹ�����1��ʧ�ܷ���0
unsigned char InsertID(MYSQL *g_conn,char *idstr)
{
	time_t t;
	struct tm *locp = NULL;
	struct tm loc;
	char sql[1024];

	for (i=0;i<1024;i++) sql[i] = 0;

	if(CheckID(g_conn,idstr) == 0)
	{
		t = time(NULL);
		locp = localtime(&t);
		
		if(locp != NULL)
			loc = *locp;
		sprintf(sql,"insert into deviceLink(_deviceID,_create_id_time) values('%s','%d-%d-%d %d:%d:%d');",idstr,(loc.tm_year+1900), (loc.tm_mon+1), loc.tm_mday,loc.tm_hour,loc.tm_min,loc.tm_sec);
		executesql(g_conn,sql);
		return CheckID(g_conn,idstr);//���ڣ�����1�����������ݳɹ�
	}
	else 
		return 0;
}

//ѡ�����ݿ⣬û�е�ʱ�򴴽�
void create_database(MYSQL *g_conn)
{
	char sql[1024];
	for (i=0;i<1024;i++) sql[i] = 0;
	sprintf(sql,"use ");
	strcat(sql,g_db_usr_name);
	strcat(sql,";");
	if(executesql(g_conn,sql) == -1)
	{
		puts("create database");
		sprintf(sql,"create database ");
		strcat(sql,g_db_usr_name);
		strcat(sql,";");
		executesql(g_conn,sql);
		print_mysql_error(g_conn,NULL);
		puts("choice database");
		sprintf(sql,"use ");
		strcat(sql,g_db_usr_name);
		strcat(sql,";");
		executesql(g_conn,sql);
		print_mysql_error(g_conn,NULL);
		puts("!!!Initialize the success!!!");
	}
	else
	{
		//executesql(sql);
		print_mysql_error(g_conn,NULL);
	}
}

