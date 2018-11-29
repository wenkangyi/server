/********************************************************************
* 标题：C语言链接mysql数据库，实现可以增删改查的角色权限登录系统
* 
* 描述：本代码可在安装GCC编译环境和mysql集成环境下直接编译运行，根据不同使用者的mysql账户可修改代码前面的内容以成功连接mysql数据库
* 编译命令：gcc mysqltest.c -lmysqlclient -o mysqltest
********************************************************************/


#include "includes.h"

//MYSQL *g_conn;//mysql 链接
//MYSQL_RES *g_res;//mysql 记录集
//MYSQL_ROW g_row;//字符串数组，mysql 记录行

#define MAX_BUF_SIZE 2048 //缓冲区最大字节数
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

int iNum_rows = 0;//mysql语句执行结果返回行数赋初值
int flag = 0;//管理员权限开关
int i = 1;//系统运行开关
pthread_mutex_t mutexMySQL=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexMySQLConn=PTHREAD_MUTEX_INITIALIZER;



//登录使用的结构体
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
//操作使用的结构体
struct Operation
{
	char tables[24];
	char name[24];
	char passwd[20];
	char role[24];
	char remark[20];
}ope;

//函数声明
void create_database(MYSQL *g_conn);
void CheckAdminUser(MYSQL *g_conn);




void ClearSqlStr(void)
{
	int i = 0;
	
	for(i=0;i<MAX_BUF_SIZE;i++)
		sql[i] = 0;
}

//打印错误信息
void print_mysql_error(MYSQL *g_conn,const char *msg)
{
	if(msg)
		printf("%s: %s\n",msg,mysql_error(g_conn));
	else
		puts(mysql_error(g_conn));
}
//执行sql语句，成功返回0，失败返回-1
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
		result = -1;//链接失败
	pthread_mutex_unlock(&mutexMySQLConn);
	//检查是否可以使用
	if(executesql(g_conn,"set names utf8"))
		result = -1;

	return result;
}

//初始化链接
int init_mysql(MYSQL *g_conn)
{		
	//connection the database
	if(!mysql_real_connect(g_conn,g_host_name,g_user_name,g_password,g_db_name,g_db_port,NULL,0))
		return -1;//链接失败
	//检查是否可以使用
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
	return 0;//返回成功
}

//select COUNT(1) as count from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='数据库名' and TABLE_NAME='表格名';
//检查表格
uInt8 CheckTable(MYSQL *g_conn,char *tableName)
{
	char sqlstr[1024];
	int _iNum_rows;
	MYSQL_RES *_g_res;
	MYSQL_ROW _g_row;
	int i = 0;
	uInt8 flag = 0;

	for(i=0;i<1024;i++) sqlstr[i] = 0;
	
	sprintf(sqlstr,"select COUNT(1) as count from INFORMATION_SCHEMA.TABLES where TABLE_SCHEMA='");//进行字符串拼接
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
		flag = atoi(_g_row[0]);//获取数据
	}
	else
	{
		flag = 0;
	}
	mysql_free_result(_g_res); // 释放结果集

	return flag;
}

void CreateTable(MYSQL *g_conn)
{
	int flag = 0;
	if(CheckTable(g_conn,"users") == 0)
	{
		//_id--
		//_name--用户名
		//_password--用户登录密码
		//_create_time--用户创建时间
		//_permissions--用户权限
		//_remark--备注
		flag = executesql(g_conn,"create table users(_id smallint unsigned primary key auto_increment,_name varchar(24) not null unique,_password char(20) not null,_create_time datetime,_permissions int,_softSocketID int default -1 not null,__remark varchar(200));");
	}
	//跟设备连接及数据转发有关
	if(CheckTable(g_conn,"deviceLink") == 0)
	{
		//_id
		//_deivceID--设备的ID
		//_deviceSocketID--设备的socketID
		//_softSocketID--客户端软件的socketID
		//_name--该设备被哪个用户所注册
		//_caption--0为新发布的ID，可用；1为禁用的ID
		flag = executesql(g_conn,"create table deviceLink(_id smallint unsigned primary key auto_increment,_deviceID varchar(30) not null unique,_create_id_time datetime,_deviceSocketID int default -1 not null,_softSocketID int default -1 not null,_name varchar(200),_use_id_time datetime,_caption int);");
	}
}

//初始化表格参数，socketFD需要初始化为-1
void init_tables(MYSQL *g_conn)
{
	executesql(g_conn,"update users set _softSocketID=-1;");
	executesql(g_conn,"update deviceLink set _deviceSocketID=-1,_softSocketID=-1;");
}

//根据得到的ID与表格内容进行判断

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

//根据deviceID获取softSocketID
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
		int iNum_fields = mysql_num_fields(_g_res); // 得到记录的列数
		_g_row=mysql_fetch_row(_g_res);
		int socketID = atoi((const char *)_g_row[socketIDFlag]);

		return socketID;
	}

	return -1;
}

//根据硬件的socket_fd获取软件的socket_fd
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
		int iNum_fields = mysql_num_fields(_g_res); // 得到记录的列数
		_g_row=mysql_fetch_row(_g_res);
		int socketID = atoi((const char *)_g_row[0]);

		return socketID;
	}

	return -1;

}

//此函数检查用户名及密码是否正确
//返回1，即为用户名错误，即不存在此用户名；
//返回2，即为用户密码错误
//返回3，即为用户名长度超过20字符
//返回4，即为用户密码长度超过18字符
//返回0，即验证正确
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
	if (buf[index] != ',') return 3;//用户名长度错误
	
	index ++;
	i = 0;
	while((buf[index] != '\r') 
		&& (i<(USER_PASSWORD_MAX_LEN- 1))){
		userPassword[i++] = buf[index++];
	}
	WriteLog(userPassword);
	if (buf[index] != '\r') return 4;//密码长度错误
	
	sprintf(sqlstr,"select _password from users where _name='%s';",userName);
	WriteLog(sqlstr);
	
	executesql(g_conn,sqlstr);
	_g_res = mysql_store_result(g_conn);
	
	_iNum_rows = mysql_num_rows(_g_res);
	if(_iNum_rows == 0) return 1;//不存在此用户名

	_g_row=mysql_fetch_row(_g_res);
	
	if(strcmp(_g_row[0],userPassword) == 0){
		//将softSocketID写入数据库
		sprintf(sqlstr,"update users set _softSocketID=%d where _name='%s';",softSocketFD,userName);
		executesql(g_conn,sqlstr);
		return 0;//验证正确
	}
	else
		return 2;//密码错误
}

//根据softSocketID从数据库中查找记录，
//在用户名登录时，需要将此参数写入数据库
//返回0，无记录，返回1，有记录
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
	if(_iNum_rows == 0) return 0;//没记录
	
	return 1;//有记录	
}

//更新成功返回0，不成功，返回1
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

//更新成功返回0，不成功，返回1
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


//上传的ID正确，即返回0，ID错误，即返回1
//如果ID长度不为 16，即返回2
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
	if(_iNum_rows == 0) return 1;//不存在此ID

	sprintf(sqlstr,"update deviceLink set _softSocketID=%d where _deviceID='%s';",socketID,deviceID);
	WriteLog(sqlstr);
	int rt = executesql(g_conn,sqlstr);
	
	if( rt == 0)
		return 0;
	return 1;
}

//检测是否存在Admin用户，不存在，即自动创建一个
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
		//添加Admin用户
		//sprintf(sql,"insert into users values(%d,'%s','%s','%s',%s,'%s');",i,ope.name,ope.passwd,Time,ID,ope.remark);
  		executesql(g_conn,"INSERT INTO users(_name, _password,_permissions) VALUES('Admin', 'Wen0136825',0);");
	}
}

//ID存在，即返回1，不存在，即返回0
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

//成功返回1，失败返回0
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
		return CheckID(g_conn,idstr);//存在，返回1，即插入数据成功
	}
	else 
		return 0;
}

//选择数据库，没有的时候创建
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

