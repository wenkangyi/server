#include "includes.h"

//变量定义
//SystemParam sysParam;
//硬件连接
SOCKET_PARAM *_socketParam = NULL;
//指向_socketParam的最后一个节点
SOCKET_PARAM *_lastDeviceNode = NULL;
//软件连接
SOFT_PARAM	*_softParam = NULL;
//指向_softParam的最后一个节点
SOFT_PARAM *_lastSoftNode = NULL;

//用于硬件接收的头节点
ssize_t gDeviceEfd,gSoftEfd;
pthread_mutex_t gDeviceEfdMutex = PTHREAD_MUTEX_INITIALIZER;
//用于软件接收的头节点
ssize_t gSoftEfd;
pthread_mutex_t gSoftEfdMutex = PTHREAD_MUTEX_INITIALIZER;



//设备缓冲池
BUFFER_POOL *_bufPoolDevice[10] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

//软件缓冲池
BUFFER_POOL *_bufPollSoft[5] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};


pthread_mutex_t socketParamMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t softParamMutex = PTHREAD_MUTEX_INITIALIZER;

//用于设备缓冲池的互斥量
pthread_mutex_t deviceBufPollMutex[10] = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
};

//用于软件终端缓冲池的互斥量
pthread_mutex_t softBufPollMutex[5] = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
};


//函数声明
static void *
ReceiveThread(void *arg);
static void *
SocketMonitorThread(void *arg);

static void *
ReceiveSoftThread(void *arg);
static void *
SocketMonitorSoftThread(void *arg);

//信号注册
void SignalRegister()
{
	signal(SIGHUP,ExceptionSIGHUP);
	signal(SIGINT,ExceptionSIGINT);
	signal(SIGQUIT,ExceptionSIGQUIT);
	signal(SIGILL,ExceptionSIGILL);
	signal(SIGTRAP,ExceptionSIGTRAP);
	signal(SIGABRT,ExceptionSIGABRT);
	signal(SIGBUS,ExceptionSIGBUS);
	signal(SIGFPE,ExceptionSIGFPE);
	signal(SIGUSR1,ExceptionSIGUSR1);
	signal(SIGSEGV,ExceptionSIGSEGV);
	signal(SIGUSR2,ExceptionSIGUSR2);
	signal(SIGPIPE,ExceptionSIGPIPE);
	signal(SIGALRM,ExceptionSIGALRM);
	signal(SIGSTKFLT,ExceptionSIGSTKFLT);
	signal(SIGCHLD,ExceptionSIGCHLD);
	signal(SIGCONT,ExceptionSIGCONT);
	signal(SIGSTOP,ExceptionSIGSTOP);
	signal(SIGTSTP,ExceptionSIGTSTP);
	signal(SIGTTIN,ExceptionSIGTTIN);
	signal(SIGTTOU,ExceptionSIGTTOU);
	signal(SIGURG,ExceptionSIGURG);
	signal(SIGXCPU,ExceptionSIGXCPU);
	signal(SIGXFSZ,ExceptionSIGXFSZ);
	signal(SIGVTALRM,ExceptionSIGVTALRM);
	signal(SIGPROF,ExceptionSIGPROF);
	signal(SIGWINCH,ExceptionSIGWINCH);
	signal(SIGIO,ExceptionSIGIO);
	signal(SIGPWR,ExceptionSIGPWR);
	signal(SIGSYS,ExceptionSIGSYS);
}

void init_buffPoll()
{
	int i=0;
	pthread_t t1;
	int *pIndex = NULL;
	char msg[200];

	memset(msg,0,200);

	//初始化缓冲池，主要是创建每个缓冲区的节点头
	for(i=0;i<10;i++)
	{
		_bufPoolDevice[i] = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
		_bufPoolDevice[i]->cmd = NULL;
		_bufPoolDevice[i]->sockfd = -1;
		_bufPoolDevice[i]->cmdLen = 0;
		_bufPoolDevice[i]->nextlink = NULL;

		sprintf(msg,"_bufPollSoft[%d] address is 0x%x***************",i,_bufPollSoft[i]);
		WriteLog(msg);
		
		if(i == 0){
			pIndex = (int *)malloc(sizeof(int));
			*pIndex = i;
			//t1是否需要管理？
			pthread_create(&t1, NULL, ReceiveThread, pIndex );
			pIndex = NULL;
		}
	}
		
	#if ADD_SOFT_COMM
	//初始化缓冲池，主要是创建每个缓冲区的节点头
	for(i=0;i<5;i++)
	{
		_bufPollSoft[i] = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
		_bufPollSoft[i]->cmd = NULL;
		_bufPollSoft[i]->sockfd = -1;
		_bufPollSoft[i]->cmdLen = 0;
		_bufPollSoft[i]->nextlink = NULL;
		
		sprintf(msg,"_bufPollSoft[%d] address is 0x%x***************",i,_bufPollSoft[i]);
		WriteLog(msg);
		
		if(i == 0){
			pIndex = (int *)malloc(sizeof(int));
			*pIndex = i;
			//t1是否需要管理？
			pthread_create(&t1, NULL, ReceiveSoftThread, pIndex );
			pIndex = NULL;
		}
	}
	#endif

}

void SystemParamInit(void)
{
	MYSQL *g_conn = NULL;//mysql 链接
	int i=0;
	pthread_t t1;
	int *pIndex = NULL;
	
	//SignalRegister();
	
	g_conn = mysql_init(NULL);
	if(init_mysql(g_conn) == 0){
		char msg[100];
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"SystemParamInit init_mysql successful!");
		WriteLog(msg);
	}
	else{
		char msg[100];
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"SystemParamInit init_mysql Error!");
		WriteLog(msg);
	}
	//初始化表格参数
	init_tables(g_conn);
	
	init_buffPoll();
}


void *
AcceptThread(void *arg)
{
    char *s = (char *) arg;
	MYSQL *g_conn = NULL;//mysql 链接
	pthread_t t1;
    int lfd;
	int opt = 1;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    int sss;
    int n, i,j;
	char log[100];//用于日志
	//连表中的最后一个节点
	BUFFER_POOL *lastNode = NULL;
	//当前创建的节点
	BUFFER_POOL *newNode = NULL;
	int connfd,sockfd;
	ssize_t nready,res;
	struct epoll_event deviceTep,gDeviceEp[SOCKET_MAX_NUM];
	char buf[MAXLINE],str[INET_ADDRSTRLEN];
	int bufIndex = 0;
	char msg[100];

	//初始化数据库连接
	g_conn = mysql_init(NULL);
	if(init_mysql_conn(g_conn) == 0){
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"device AcceptThread init_mysql successful!");
		WriteLog(msg);
	}
	else{
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"device AcceptThread init_mysql Error!");
		WriteLog(msg);
	}
	
	#if SYS_DEBUG
	printf("device AcceptThread %s\n",s);
	#endif
    lfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//端口复用
	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(lfd, SOCKET_MAX_NUM);
	//----
	//创建池
	gDeviceEfd = epoll_create(SOCKET_MAX_NUM);
	
	deviceTep.events = EPOLLIN;
	deviceTep.data.fd = lfd;
	res = epoll_ctl(gDeviceEfd,EPOLL_CTL_ADD,lfd,&deviceTep);
	//----
    clie_addr_len = sizeof(clie_addr);

	//创建监控线程
  	//sss = pthread_create(&t1, NULL, SocketMonitorThread, NULL );
	

	while(1)
	{
		int ss;
		//pthread_mutex_lock(&gDeviceEfdMutex);
		//如果连接数超出OPEN_MAX，但我还想连接当前端口，应该怎样处理？
		nready = epoll_wait(gDeviceEfd,gDeviceEp,SOCKET_MAX_NUM,-1);//最后一个参数为-1，是为无限等待
		//pthread_mutex_unlock(&gDeviceEfdMutex);
		
		sprintf(&log[0],"online device num: %ld\n",nready);
		
		#if SYS_DEBUG
		printf("%s",log);
		#endif
		WriteLog(log);
		
		for(i=0;i<nready;i++){
			if(gDeviceEp[i].data.fd == lfd){
				connfd = accept(lfd,(struct sockaddr *)&clie_addr,&clie_addr_len);
				#if SYS_DEBUG
				printf("received from %s at port %d\n",inet_ntop(AF_INET,&clie_addr.sin_addr,str,sizeof(str)),clie_addr.sin_port);
				#endif
				//将监听到的连接添加到epoll池中
				deviceTep.events = EPOLLIN;
				deviceTep.data.fd = connfd,
				pthread_mutex_lock(&gDeviceEfdMutex);
				res = epoll_ctl(gDeviceEfd,EPOLL_CTL_ADD,connfd,&deviceTep);
				pthread_mutex_unlock(&gDeviceEfdMutex);
				//发送读ID命令
				write(connfd, "AT+WID=?\r\n", 10);
			}
			else{
				sockfd = gDeviceEp[i].data.fd;
				//这个API在此场景下为什么会变成非阻塞状态？
				n = read(sockfd,buf,MAXLINE);
				if(n == 0){
					//需要测试，硬件自动断电时，是否还能执行这里
					
					ClearDeviceSocketFD(g_conn,sockfd);
					pthread_mutex_lock(&gDeviceEfdMutex);
					res = epoll_ctl(gDeviceEfd,EPOLL_CTL_DEL,sockfd,NULL);
					pthread_mutex_unlock(&gDeviceEfdMutex);
					close(sockfd);
					#if SYS_DEBUG
					printf("client[%d] closed connection\n",i);
					#endif
				}
				else if(n < 0){
					//是否需要处理这种情况，接收到的数据小于0，该怎样处理？
					
				}
				else{
					#if SYS_DEBUG
					printf("recv new device data\n");
					#endif
					//申请缓冲节点内存
					newNode = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
					//申请新的命令内存
					newNode->cmd = malloc(n);
					//将接收到的数据复制到新申请的内存中
					memcpy(newNode->cmd,buf,n);
					//接收到的命令长度
					newNode->cmdLen = n;
					//复制socket描述符
					newNode->sockfd = sockfd;
					//下一个节点为空
					newNode->nextlink = NULL;
					
					//进入节点操作前，进行阻塞操作
					pthread_mutex_lock(&deviceBufPollMutex[bufIndex]);
					//将节点提交到缓冲池中
					lastNode = _bufPoolDevice[bufIndex];
					//先遍历缓冲池，直到最后一个节点
					while(lastNode->nextlink != NULL)
						lastNode = lastNode->nextlink;
					lastNode->nextlink = newNode;
					pthread_mutex_unlock(&deviceBufPollMutex[bufIndex]);
					newNode = NULL;
					bufIndex ++;//10
					if(bufIndex >=1) bufIndex=0;
					//将接收到的数据提交到队列中，由其它线程处理
				}
			}
		}
	}	
	#if SYS_DEBUG
	printf("AcceptThread Exit!\r\n");
	#endif
    return (void *) strlen(s);
}

static void *
SocketMonitorThread(void *arg)
{
	uInt32 i = 0;
	Int32 s;
	SOCKET_PARAM *currNode = NULL;
	SOCKET_PARAM *freeNode = NULL;
	SOCKET_PARAM *nextNode = NULL;
	SOCKET_PARAM *calcNode = NULL;
	char logStr[50];

	#if SYS_DEBUG
	printf("SocketMonitorThread\n");
	#endif
	WriteLog("SocketMonitorThread\r\n");
	
	while(1)
	{
		if(_socketParam->nextlink == NULL) continue;
		sleep(1);
		
		currNode = _socketParam;
		
		i = 0;
		do{
			
			nextNode = currNode->nextlink;
			i ++;
			if((nextNode->_threadStatus == _threadRUN)
			||(nextNode->_threadStatus == _threadEXIT))
			{
				nextNode->_timerout ++;
				//释放线程  关闭连接  释放内存
				if(nextNode->_timerout >= TIMEROUT_TICKS)
				{
					nextNode->_timerout = 0;
				//在此释放的过程中需要上锁
					if(nextNode->_threadStatus == _threadRUN)
						s = pthread_cancel(nextNode->_receiveThreadID);
					#if SYS_DEBUG
					printf("pthread_cancel( %ld)\n",nextNode->_receiveThreadID);
					#endif
					nextNode->_threadStatus = _threadEND;
					
					close(nextNode->_socketFD);
					//ClearDeviceSocketFD(nextNode->_socketFD);
					
					pthread_mutex_lock(&socketParamMutex);
					freeNode = nextNode;
					if(nextNode->nextlink == NULL){
						currNode->nextlink = NULL;
						_lastDeviceNode = currNode;
					}
					else{
						nextNode = nextNode->nextlink;
						currNode->nextlink = nextNode;
					}
					free(freeNode);
					pthread_mutex_unlock(&socketParamMutex);
					if (i>0) i--;
				}
			}
			
			if(currNode->nextlink != NULL)
				currNode = currNode->nextlink;
			
		}while(currNode->nextlink!=NULL);
		
		
		sprintf(logStr,"inline num: %d\n",i);

		//获取系统日期，以日期为文件名
		#if SYS_DEBUG
		printf("%s",logStr);
		#endif
		WriteLog(logStr);
		
	}
}

static void *
ReceiveThread(void *arg)
{
	//SOCKET_PARAM *sp = ((SOCKET_PARAM *)arg);
	int *devicePollIndex = (int *)arg;
	MYSQL *g_conn = NULL;//mysql 链接
	int i = 0;
	int rt = 0;
	char judgeID = 0;
	char deviceID[ID_LEN];
	int socketID = 0;
	char *pos = NULL;
	BUFFER_POOL *currNode = NULL;
	char msg[100];
	
	g_conn = mysql_init(NULL);
	if(init_mysql_conn(g_conn) == 0){
		
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"device thread:%d init_mysql successful!",*devicePollIndex);
		WriteLog(msg);
	}
	else{
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"device thread:%d init_mysql Error!",*devicePollIndex);
		WriteLog(msg);
	}
	
	while(1)
	{
		//获取节点头
		currNode = _bufPoolDevice[*devicePollIndex];
		if(currNode->nextlink == NULL) continue;
		currNode = currNode->nextlink;
		
		#if SYS_DEBUG
		printf("devide socketFD:%d Rece:%s\r\n",currNode->sockfd, currNode->cmd);
		#endif
		WriteLog(currNode->cmd);
		
		//下面进行ID的判断
		if(strstr(currNode->cmd,"+WID:") != 0){
			pos = strstr(currNode->cmd,"+WID:");//如果有心跳的话，可能会粘包
			//if(n == (7 + 16))//+WID:....\r\n所以为7+16
			if(pos != 0){
				#if SYS_DEBUG
				printf("g_conn = %ld \r\n",(long int)g_conn);
				#endif
				for(i=0;i<ID_LEN;i++) deviceID[i] = 0;
				rt = StringSub(&pos[5],deviceID,ID_LEN);
				pos = NULL;
				if(rt == 0){
					WriteLog("ID ERROR, exit Thread!");
					//数据不对，关闭连接
					close(currNode->sockfd);
					//需要释放epoll中的资源
					pthread_mutex_lock(&gDeviceEfdMutex);
					epoll_ctl(gDeviceEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
					pthread_mutex_unlock(&gDeviceEfdMutex);
					sprintf(msg,"device%d +WID: Error !\r\n",*devicePollIndex);
					WriteLog(msg);
				}
				//将得到的ID跟数据库对比
				else if (JudgeIDToDeviceLink(g_conn,deviceID,currNode->sockfd) == 0){
					WriteLog("Judge ID ERROR, exit Thread!");
					//ID不对，关闭连接
					close(currNode->sockfd);
					//需要释放epoll中的资源
					pthread_mutex_lock(&gDeviceEfdMutex);
					epoll_ctl(gDeviceEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
					pthread_mutex_unlock(&gDeviceEfdMutex);
					sprintf(msg,"device%d not deviceID !\r\n",*devicePollIndex);
					WriteLog(msg);
				}
				else {
					WriteLog("The ID is good,Run!");
				}
			}
		}
		else if(strstr(currNode->cmd,"AT\r\n") != 0)
		{
			write(currNode->sockfd, "AT+ACK\r\n", 8);
		}
		else{
			//下面是透传协议的处理
			#if defined(AGREEMENT_PASSTHROUGH_MODE)
			socketID = InDeviceSockFDOutSoftSockFD(g_conn,currNode->sockfd);//GetSoftSocketID(g_conn,deviceID,_SOFT_SOCKET_ID);
			if(socketID < 0){
				close(currNode->sockfd);
				//需要释放epoll中的资源
				pthread_mutex_lock(&gDeviceEfdMutex);
				epoll_ctl(gDeviceEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gDeviceEfdMutex);
				sprintf(msg,"device%d not deviceSocketFD !\r\n",*devicePollIndex);
				WriteLog(msg);
			}
			else{
				/*if(strstr(currNode->cmd,"AT\r\n") != 0)
				{
					write(currNode->sockfd, "AT+ACK\r\n", 8);
				}
				else {*/
					#if SYS_DEBUG
					printf("device socketFD:%d Send:%s\r\n",socketID, currNode->cmd);
					#endif
					write(socketID, currNode->cmd, currNode->cmdLen);//转发
				//}
			}
			#endif
		}
		
		//删除节点
		//在删除前，需要有阻塞动作
		pthread_mutex_lock(&deviceBufPollMutex[*devicePollIndex]);
		_bufPoolDevice[*devicePollIndex]->nextlink = currNode->nextlink;
		free(currNode->cmd);
		currNode->cmd = NULL;
		free(currNode);
		currNode = NULL;
		pthread_mutex_unlock(&deviceBufPollMutex[*devicePollIndex]);
	}
}

#if ADD_SOFT_COMM
void *
AcceptSoftThread(void *arg)
{
	char *s = (char *) arg;
	MYSQL *g_conn = NULL;//mysql 链接
	pthread_t t1;
	int opt = 1;
    int lfd;
	char log[100];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    int sss;
    int n, i;
	//SOFT_PARAM *lastNode = NULL;
	//SOFT_PARAM *newNode = NULL;
	//连表中的最后一个节点
	BUFFER_POOL *lastNode = NULL;
	//当前创建的节点
	BUFFER_POOL *newNode = NULL;
	int connfd,sockfd;
	ssize_t nready,res;
	struct epoll_event tep,ep[SOCKET_MAX_NUM];
	char buf[MAXLINE],str[INET_ADDRSTRLEN];
	int *pIndex = NULL;
	int bufIndex = 0;
	char msg[100];
	
	//初始化数据库连接
	g_conn = mysql_init(NULL);
	if(init_mysql_conn(g_conn) == 0){
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"soft AcceptThread init_mysql successful!");
		WriteLog(msg);
	}
	else{
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"soft AcceptThread init_mysql Error!");
		WriteLog(msg);
	}
	

	#if SYS_DEBUG
	printf("Soft AcceptThread %s\n",s);
	#endif
    lfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SOFT_SERV_PORT); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//端口复用
	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(lfd, SOFT_SOCKET_MAX_NUM);
	//----
	//创建池
	gSoftEfd = epoll_create(SOCKET_MAX_NUM);
	
	tep.events = EPOLLIN;
	tep.data.fd = lfd;
	res = epoll_ctl(gSoftEfd,EPOLL_CTL_ADD,lfd,&tep);
	//----

    clie_addr_len = sizeof(clie_addr);

	
  	/*sss = pthread_create(&t1, NULL, SocketMonitorSoftThread, NULL );
	*/
	
	while(1)
	{
		int ss;
		//如果连接数超出OPEN_MAX，但我还想连接当前端口，应该怎样处理？
		//pthread_mutex_lock(&gSoftEfdMutex);
		nready = epoll_wait(gSoftEfd,ep,SOCKET_MAX_NUM,-1);//最后一个参数为-1，是为无限等待
		//pthread_mutex_unlock(&gSoftEfdMutex);
		
		sprintf(&log[0],"online soft num: %ld\n",nready);
		
		#if SYS_DEBUG
		printf("%s",log);
		#endif
		WriteLog(log);
		
		for(i=0;i<nready;i++){
			if(ep[i].data.fd == lfd){
				connfd = accept(lfd,(struct sockaddr *)&clie_addr,&clie_addr_len);
				#if SYS_DEBUG
				printf("received from %s at port %d\n",inet_ntop(AF_INET,&clie_addr.sin_addr,str,sizeof(str)),clie_addr.sin_port);
				#endif
				//将监听到的连接添加到epoll池中
				tep.events = EPOLLIN;
				tep.data.fd = connfd,
				pthread_mutex_lock(&gSoftEfdMutex);
				res = epoll_ctl(gSoftEfd,EPOLL_CTL_ADD,connfd,&tep);
				pthread_mutex_unlock(&gSoftEfdMutex);
			}
			else{
				sockfd = ep[i].data.fd;
				//这个API在此场景下为什么会变成非阻塞状态？
				n = read(sockfd,buf,MAXLINE);
				if(n == 0){
					//需要测试，硬件自动断电时，是否还能执行这里
					ClearSoftSocketFD(g_conn,sockfd);
					pthread_mutex_lock(&gSoftEfdMutex);
					res = epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,sockfd,NULL);
					pthread_mutex_unlock(&gSoftEfdMutex);
					close(sockfd);
					#if SYS_DEBUG
					printf("client[%d] closed connection\n",i);
					#endif
				}
				else if(n < 0){
					//是否需要处理这种情况，接收到的数据小于0，该怎样处理？
					
				}
				else{
					#if SYS_DEBUG
					printf("recv new soft data\n");
					#endif
					//申请缓冲节点内存
					newNode = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
					//申请新的命令内存
					newNode->cmd = malloc(n);
					//将接收到的数据复制到新申请的内存中
					memcpy(newNode->cmd,buf,n);
					//接收到的命令长度
					newNode->cmdLen = n;
					//复制socket描述符
					newNode->sockfd = sockfd;
					//下一个节点为空
					newNode->nextlink = NULL;

					sprintf(msg,"soft newNode address is 0x%x***************",newNode);
					WriteLog(msg);
					sprintf(msg,"cmd address is 0x%x***************",newNode->cmd);
					WriteLog(msg);
					
					//进入节点操作前，进行阻塞操作
					pthread_mutex_lock(&softBufPollMutex[bufIndex]);
					//将节点提交到缓冲池中
					lastNode = _bufPollSoft[bufIndex];
					//先遍历缓冲池，直到最后一个节点
					while(lastNode->nextlink != NULL)
						lastNode = lastNode->nextlink;
					lastNode->nextlink = newNode;
					pthread_mutex_unlock(&softBufPollMutex[bufIndex]);
					newNode = NULL;
					
					bufIndex ++;//5
					if(bufIndex>=1) bufIndex = 0;
					//将接收到的数据提交到队列中，由其它线程处理
				}
			}
		}
	}	

    return (void *) strlen(s);
}



static void *
ReceiveSoftThread(void *arg)
{
	//SOFT_PARAM *sp = ((SOFT_PARAM *)arg);
	
	int *softPollIndex = (int *)arg;
	MYSQL *g_conn = NULL;//mysql 链接
	int i = 0;
	char buf[BUFSIZ];//接收缓存
	char userCheckFlag = 0;
	int socketID = 0;
	char deviceID[ID_LEN];
	char *pos = NULL;
	BUFFER_POOL *currNode = NULL;
	char msg[100];
	
	g_conn = mysql_init(NULL);
	if(init_mysql_conn(g_conn) == 0){
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"soft thread:%d init_mysql successful!",*softPollIndex);
		WriteLog(msg);
	}
	else{
		for(i=0;i<100;i++)
			msg[i] = 0;
		sprintf(msg,"soft thread:%d init_mysql Error!",*softPollIndex);
		WriteLog(msg);
	}
		
	while(1)
	{
		
		//获取节点头
		currNode = _bufPollSoft[*softPollIndex];
		if(currNode->nextlink == NULL) continue;
		currNode = currNode->nextlink;
		
		#if SYS_DEBUG
		printf("soft socketFD:%d Rece:%s\r\n",currNode->sockfd, currNode->cmd);
		#endif
		WriteLog(currNode->cmd);
		

		//提取用户名与密码
		//指令 AT+USERS=
		//用户名最大支持20字符，密码最大支持18字符
		if(strstr(currNode->cmd,"AT+USERS=") != 0){
			pos = strstr(currNode->cmd,"AT+USERS=");
			if (pos == 0){
				write(currNode->sockfd, "ERROR\r\n", strlen("ERROR\r\n"));
				close(currNode->sockfd);
				pthread_mutex_lock(&gSoftEfdMutex);
				epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gSoftEfdMutex);
			}
			else{
				//处理登录指令
				int flag = CheckUserName(g_conn,&pos[9],currNode->sockfd);
				if(flag == 0) {
					write(currNode->sockfd, "OK\r\n", strlen("OK\r\n"));
				}
				else{
					sprintf(buf,"ERROR%d\r\n",flag);
					write(currNode->sockfd, buf, strlen(buf));
					close(currNode->sockfd);
					pthread_mutex_lock(&gSoftEfdMutex);
					epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
					pthread_mutex_unlock(&gSoftEfdMutex);
				}
			}
		}
		else if(strstr(currNode->cmd,"AT+ID=") != 0){
			//通过用户名与ID关联
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//踢掉连接
				close(currNode->sockfd);
				pthread_mutex_lock(&gSoftEfdMutex);
				epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gSoftEfdMutex);
				sprintf(msg,"soft%d not softSocketFD to Users!\r\n",*softPollIndex);
				WriteLog(msg);
			}
			else{
				//命令AT+ID=0123456789ABCDEF\r\n
				//传入ID的同时，也需要传入socketID
				int rt = RecordSocketID(g_conn,&currNode->cmd[6],currNode->sockfd);
				if(rt == 0)
					write(currNode->sockfd, "OK\r\n", strlen("OK\r\n"));
				else
					write(currNode->sockfd, "ERROR\r\n", strlen("ERROR\r\n"));
			}
		}
		else if(strstr(currNode->cmd,"AT\r\n") != 0){
			//通过用户名与ID关联
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//踢掉连接
				close(currNode->sockfd);
				pthread_mutex_lock(&gSoftEfdMutex);
				epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gSoftEfdMutex);
				sprintf(msg,"soft%d not softSocketFD to Users!\r\n",*softPollIndex);
				WriteLog(msg);
			}
			else{
				write(currNode->sockfd,"AT+ACK\r\n",8);
			}
		}
		else{
			//通过用户名与ID关联
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//踢掉连接
				close(currNode->sockfd);
				pthread_mutex_lock(&gSoftEfdMutex);
				epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gSoftEfdMutex);
				sprintf(msg,"soft%d not softSocketFD to Users!\r\n",*softPollIndex);
				WriteLog(msg);
			}
			else{
				#if defined(AGREEMENT_PASSTHROUGH_MODE)
				StringSub(&currNode->cmd[1],deviceID,ID_LEN);
				socketID = GetSoftSocketID(g_conn,deviceID,_DEVICE_SOCKET_ID);
				if(socketID > 0)
				{
					#if SYS_DEBUG
					printf("soft socketFD:%d Send:%s\r\n",socketID, &currNode->cmd[19]);
					#endif
					//转发，在这之前如何测试一个文件描述符的有效性
					write(socketID, &currNode->cmd[19], currNode->cmdLen-(ID_LEN+2));
					//write(currNode->sockfd,"OK\r\n",4);
				}
				else
				{
					write(currNode->sockfd,"ERROR\r\n",7);
				}
				#endif
			}
		}

		sprintf(msg,"softThread currNode address is 0x%x***************",currNode);
		WriteLog(msg);
		sprintf(msg,"softThread cmd address is 0x%x***************",currNode->cmd);
		WriteLog(msg);
		//删除节点
		//在删除前，需要有阻塞动作
		pthread_mutex_lock(&softBufPollMutex[*softPollIndex]);
		_bufPollSoft[*softPollIndex]->nextlink = currNode->nextlink;
		free(currNode->cmd);
		currNode->cmd = NULL;
		free(currNode);
		currNode = NULL;
		pthread_mutex_unlock(&softBufPollMutex[*softPollIndex]);
	}
}

static void *
SocketMonitorSoftThread(void *arg)
{
	uInt32 i = 0;
	Int32 s;
	SOFT_PARAM *currNode = NULL;
	SOFT_PARAM *freeNode = NULL;
	SOFT_PARAM *nextNode = NULL;
	SOFT_PARAM *calcNode = NULL;
	char logStr[50];

	#if SYS_DEBUG
	printf("SocketMonitorSoftThread\n");
	#endif
	WriteLog("SocketMonitorSoftThread\r\n");
	
	while(1)
	{
		if(_softParam->nextlink == NULL) continue;
		sleep(1);
		
		currNode = _softParam;
		i = 0;
		do{
			
			nextNode = currNode->nextlink;
			i ++;
			if((nextNode->_threadStatus == _threadRUN)
			||(nextNode->_threadStatus == _threadEXIT))
			{
				nextNode->_timerout ++;
				
				if(nextNode->_timerout >= TIMEROUT_TICKS)
				{
					nextNode->_timerout = 0;
					
					if(nextNode->_threadStatus == _threadRUN)
						s = pthread_cancel(nextNode->_receiveThreadID);
					#if SYS_DEBUG
					printf("pthread_cancel( %ld)\n",nextNode->_receiveThreadID);
					#endif
					nextNode->_threadStatus = _threadEND;
					
					close(nextNode->_socketFD);
					//ClearSoftSocketFD(nextNode->_socketFD);

					pthread_mutex_lock(&softParamMutex);
					freeNode = nextNode;
					if(nextNode->nextlink == NULL){
						currNode->nextlink = NULL;
						_lastSoftNode = currNode;
					}
					else{
						nextNode = nextNode->nextlink;
						currNode->nextlink = nextNode;
					}
					free(freeNode);
					pthread_mutex_unlock(&softParamMutex);
					if (i>0) i--;
				}
			}
			if(currNode->nextlink != NULL)
				currNode = currNode->nextlink;
		}while(currNode->nextlink!=NULL);

		sprintf(logStr,"soft inline num: %d\n",i);

		//获取系统日期，以日期为文件名
		#if SYS_DEBUG
		printf("%s",logStr);
		#endif
		WriteLog(logStr);
		
	}
}

#endif

