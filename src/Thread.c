#include "includes.h"

//��������
//SystemParam sysParam;
//Ӳ������
SOCKET_PARAM *_socketParam = NULL;
//ָ��_socketParam�����һ���ڵ�
SOCKET_PARAM *_lastDeviceNode = NULL;
//�������
SOFT_PARAM	*_softParam = NULL;
//ָ��_softParam�����һ���ڵ�
SOFT_PARAM *_lastSoftNode = NULL;

//����Ӳ�����յ�ͷ�ڵ�
ssize_t gDeviceEfd,gSoftEfd;
pthread_mutex_t gDeviceEfdMutex = PTHREAD_MUTEX_INITIALIZER;
//����������յ�ͷ�ڵ�
ssize_t gSoftEfd;
pthread_mutex_t gSoftEfdMutex = PTHREAD_MUTEX_INITIALIZER;



//�豸�����
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

//��������
BUFFER_POOL *_bufPollSoft[5] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};


pthread_mutex_t socketParamMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t softParamMutex = PTHREAD_MUTEX_INITIALIZER;

//�����豸����صĻ�����
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

//��������ն˻���صĻ�����
pthread_mutex_t softBufPollMutex[5] = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
};


//��������
static void *
ReceiveThread(void *arg);
static void *
SocketMonitorThread(void *arg);

static void *
ReceiveSoftThread(void *arg);
static void *
SocketMonitorSoftThread(void *arg);

//�ź�ע��
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

	//��ʼ������أ���Ҫ�Ǵ���ÿ���������Ľڵ�ͷ
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
			//t1�Ƿ���Ҫ����
			pthread_create(&t1, NULL, ReceiveThread, pIndex );
			pIndex = NULL;
		}
	}
		
	#if ADD_SOFT_COMM
	//��ʼ������أ���Ҫ�Ǵ���ÿ���������Ľڵ�ͷ
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
			//t1�Ƿ���Ҫ����
			pthread_create(&t1, NULL, ReceiveSoftThread, pIndex );
			pIndex = NULL;
		}
	}
	#endif

}

void SystemParamInit(void)
{
	MYSQL *g_conn = NULL;//mysql ����
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
	//��ʼ��������
	init_tables(g_conn);
	
	init_buffPoll();
}


void *
AcceptThread(void *arg)
{
    char *s = (char *) arg;
	MYSQL *g_conn = NULL;//mysql ����
	pthread_t t1;
    int lfd;
	int opt = 1;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    int sss;
    int n, i,j;
	char log[100];//������־
	//�����е����һ���ڵ�
	BUFFER_POOL *lastNode = NULL;
	//��ǰ�����Ľڵ�
	BUFFER_POOL *newNode = NULL;
	int connfd,sockfd;
	ssize_t nready,res;
	struct epoll_event deviceTep,gDeviceEp[SOCKET_MAX_NUM];
	char buf[MAXLINE],str[INET_ADDRSTRLEN];
	int bufIndex = 0;
	char msg[100];

	//��ʼ�����ݿ�����
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

	//�˿ڸ���
	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(lfd, SOCKET_MAX_NUM);
	//----
	//������
	gDeviceEfd = epoll_create(SOCKET_MAX_NUM);
	
	deviceTep.events = EPOLLIN;
	deviceTep.data.fd = lfd;
	res = epoll_ctl(gDeviceEfd,EPOLL_CTL_ADD,lfd,&deviceTep);
	//----
    clie_addr_len = sizeof(clie_addr);

	//��������߳�
  	//sss = pthread_create(&t1, NULL, SocketMonitorThread, NULL );
	

	while(1)
	{
		int ss;
		//pthread_mutex_lock(&gDeviceEfdMutex);
		//�������������OPEN_MAX�����һ������ӵ�ǰ�˿ڣ�Ӧ����������
		nready = epoll_wait(gDeviceEfd,gDeviceEp,SOCKET_MAX_NUM,-1);//���һ������Ϊ-1����Ϊ���޵ȴ�
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
				//����������������ӵ�epoll����
				deviceTep.events = EPOLLIN;
				deviceTep.data.fd = connfd,
				pthread_mutex_lock(&gDeviceEfdMutex);
				res = epoll_ctl(gDeviceEfd,EPOLL_CTL_ADD,connfd,&deviceTep);
				pthread_mutex_unlock(&gDeviceEfdMutex);
				//���Ͷ�ID����
				write(connfd, "AT+WID=?\r\n", 10);
			}
			else{
				sockfd = gDeviceEp[i].data.fd;
				//���API�ڴ˳�����Ϊʲô���ɷ�����״̬��
				n = read(sockfd,buf,MAXLINE);
				if(n == 0){
					//��Ҫ���ԣ�Ӳ���Զ��ϵ�ʱ���Ƿ���ִ������
					
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
					//�Ƿ���Ҫ����������������յ�������С��0������������
					
				}
				else{
					#if SYS_DEBUG
					printf("recv new device data\n");
					#endif
					//���뻺��ڵ��ڴ�
					newNode = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
					//�����µ������ڴ�
					newNode->cmd = malloc(n);
					//�����յ������ݸ��Ƶ���������ڴ���
					memcpy(newNode->cmd,buf,n);
					//���յ��������
					newNode->cmdLen = n;
					//����socket������
					newNode->sockfd = sockfd;
					//��һ���ڵ�Ϊ��
					newNode->nextlink = NULL;
					
					//����ڵ����ǰ��������������
					pthread_mutex_lock(&deviceBufPollMutex[bufIndex]);
					//���ڵ��ύ���������
					lastNode = _bufPoolDevice[bufIndex];
					//�ȱ�������أ�ֱ�����һ���ڵ�
					while(lastNode->nextlink != NULL)
						lastNode = lastNode->nextlink;
					lastNode->nextlink = newNode;
					pthread_mutex_unlock(&deviceBufPollMutex[bufIndex]);
					newNode = NULL;
					bufIndex ++;//10
					if(bufIndex >=1) bufIndex=0;
					//�����յ��������ύ�������У��������̴߳���
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
				//�ͷ��߳�  �ر�����  �ͷ��ڴ�
				if(nextNode->_timerout >= TIMEROUT_TICKS)
				{
					nextNode->_timerout = 0;
				//�ڴ��ͷŵĹ�������Ҫ����
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

		//��ȡϵͳ���ڣ�������Ϊ�ļ���
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
	MYSQL *g_conn = NULL;//mysql ����
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
		//��ȡ�ڵ�ͷ
		currNode = _bufPoolDevice[*devicePollIndex];
		if(currNode->nextlink == NULL) continue;
		currNode = currNode->nextlink;
		
		#if SYS_DEBUG
		printf("devide socketFD:%d Rece:%s\r\n",currNode->sockfd, currNode->cmd);
		#endif
		WriteLog(currNode->cmd);
		
		//�������ID���ж�
		if(strstr(currNode->cmd,"+WID:") != 0){
			pos = strstr(currNode->cmd,"+WID:");//����������Ļ������ܻ�ճ��
			//if(n == (7 + 16))//+WID:....\r\n����Ϊ7+16
			if(pos != 0){
				#if SYS_DEBUG
				printf("g_conn = %ld \r\n",(long int)g_conn);
				#endif
				for(i=0;i<ID_LEN;i++) deviceID[i] = 0;
				rt = StringSub(&pos[5],deviceID,ID_LEN);
				pos = NULL;
				if(rt == 0){
					WriteLog("ID ERROR, exit Thread!");
					//���ݲ��ԣ��ر�����
					close(currNode->sockfd);
					//��Ҫ�ͷ�epoll�е���Դ
					pthread_mutex_lock(&gDeviceEfdMutex);
					epoll_ctl(gDeviceEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
					pthread_mutex_unlock(&gDeviceEfdMutex);
					sprintf(msg,"device%d +WID: Error !\r\n",*devicePollIndex);
					WriteLog(msg);
				}
				//���õ���ID�����ݿ�Ա�
				else if (JudgeIDToDeviceLink(g_conn,deviceID,currNode->sockfd) == 0){
					WriteLog("Judge ID ERROR, exit Thread!");
					//ID���ԣ��ر�����
					close(currNode->sockfd);
					//��Ҫ�ͷ�epoll�е���Դ
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
			//������͸��Э��Ĵ���
			#if defined(AGREEMENT_PASSTHROUGH_MODE)
			socketID = InDeviceSockFDOutSoftSockFD(g_conn,currNode->sockfd);//GetSoftSocketID(g_conn,deviceID,_SOFT_SOCKET_ID);
			if(socketID < 0){
				close(currNode->sockfd);
				//��Ҫ�ͷ�epoll�е���Դ
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
					write(socketID, currNode->cmd, currNode->cmdLen);//ת��
				//}
			}
			#endif
		}
		
		//ɾ���ڵ�
		//��ɾ��ǰ����Ҫ����������
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
	MYSQL *g_conn = NULL;//mysql ����
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
	//�����е����һ���ڵ�
	BUFFER_POOL *lastNode = NULL;
	//��ǰ�����Ľڵ�
	BUFFER_POOL *newNode = NULL;
	int connfd,sockfd;
	ssize_t nready,res;
	struct epoll_event tep,ep[SOCKET_MAX_NUM];
	char buf[MAXLINE],str[INET_ADDRSTRLEN];
	int *pIndex = NULL;
	int bufIndex = 0;
	char msg[100];
	
	//��ʼ�����ݿ�����
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
	//�˿ڸ���
	setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&opt,sizeof(opt));
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    listen(lfd, SOFT_SOCKET_MAX_NUM);
	//----
	//������
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
		//�������������OPEN_MAX�����һ������ӵ�ǰ�˿ڣ�Ӧ����������
		//pthread_mutex_lock(&gSoftEfdMutex);
		nready = epoll_wait(gSoftEfd,ep,SOCKET_MAX_NUM,-1);//���һ������Ϊ-1����Ϊ���޵ȴ�
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
				//����������������ӵ�epoll����
				tep.events = EPOLLIN;
				tep.data.fd = connfd,
				pthread_mutex_lock(&gSoftEfdMutex);
				res = epoll_ctl(gSoftEfd,EPOLL_CTL_ADD,connfd,&tep);
				pthread_mutex_unlock(&gSoftEfdMutex);
			}
			else{
				sockfd = ep[i].data.fd;
				//���API�ڴ˳�����Ϊʲô���ɷ�����״̬��
				n = read(sockfd,buf,MAXLINE);
				if(n == 0){
					//��Ҫ���ԣ�Ӳ���Զ��ϵ�ʱ���Ƿ���ִ������
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
					//�Ƿ���Ҫ����������������յ�������С��0������������
					
				}
				else{
					#if SYS_DEBUG
					printf("recv new soft data\n");
					#endif
					//���뻺��ڵ��ڴ�
					newNode = (BUFFER_POOL *)malloc(sizeof(BUFFER_POOL));
					//�����µ������ڴ�
					newNode->cmd = malloc(n);
					//�����յ������ݸ��Ƶ���������ڴ���
					memcpy(newNode->cmd,buf,n);
					//���յ��������
					newNode->cmdLen = n;
					//����socket������
					newNode->sockfd = sockfd;
					//��һ���ڵ�Ϊ��
					newNode->nextlink = NULL;

					sprintf(msg,"soft newNode address is 0x%x***************",newNode);
					WriteLog(msg);
					sprintf(msg,"cmd address is 0x%x***************",newNode->cmd);
					WriteLog(msg);
					
					//����ڵ����ǰ��������������
					pthread_mutex_lock(&softBufPollMutex[bufIndex]);
					//���ڵ��ύ���������
					lastNode = _bufPollSoft[bufIndex];
					//�ȱ�������أ�ֱ�����һ���ڵ�
					while(lastNode->nextlink != NULL)
						lastNode = lastNode->nextlink;
					lastNode->nextlink = newNode;
					pthread_mutex_unlock(&softBufPollMutex[bufIndex]);
					newNode = NULL;
					
					bufIndex ++;//5
					if(bufIndex>=1) bufIndex = 0;
					//�����յ��������ύ�������У��������̴߳���
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
	MYSQL *g_conn = NULL;//mysql ����
	int i = 0;
	char buf[BUFSIZ];//���ջ���
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
		
		//��ȡ�ڵ�ͷ
		currNode = _bufPollSoft[*softPollIndex];
		if(currNode->nextlink == NULL) continue;
		currNode = currNode->nextlink;
		
		#if SYS_DEBUG
		printf("soft socketFD:%d Rece:%s\r\n",currNode->sockfd, currNode->cmd);
		#endif
		WriteLog(currNode->cmd);
		

		//��ȡ�û���������
		//ָ�� AT+USERS=
		//�û������֧��20�ַ����������֧��18�ַ�
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
				//�����¼ָ��
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
			//ͨ���û�����ID����
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//�ߵ�����
				close(currNode->sockfd);
				pthread_mutex_lock(&gSoftEfdMutex);
				epoll_ctl(gSoftEfd,EPOLL_CTL_DEL,currNode->sockfd,NULL);
				pthread_mutex_unlock(&gSoftEfdMutex);
				sprintf(msg,"soft%d not softSocketFD to Users!\r\n",*softPollIndex);
				WriteLog(msg);
			}
			else{
				//����AT+ID=0123456789ABCDEF\r\n
				//����ID��ͬʱ��Ҳ��Ҫ����socketID
				int rt = RecordSocketID(g_conn,&currNode->cmd[6],currNode->sockfd);
				if(rt == 0)
					write(currNode->sockfd, "OK\r\n", strlen("OK\r\n"));
				else
					write(currNode->sockfd, "ERROR\r\n", strlen("ERROR\r\n"));
			}
		}
		else if(strstr(currNode->cmd,"AT\r\n") != 0){
			//ͨ���û�����ID����
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//�ߵ�����
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
			//ͨ���û�����ID����
			if(JudgeSocketFDUserName(g_conn,currNode->sockfd) == 0){
				//�ߵ�����
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
					//ת��������֮ǰ��β���һ���ļ�����������Ч��
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
		//ɾ���ڵ�
		//��ɾ��ǰ����Ҫ����������
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

		//��ȡϵͳ���ڣ�������Ϊ�ļ���
		#if SYS_DEBUG
		printf("%s",logStr);
		#endif
		WriteLog(logStr);
		
	}
}

#endif

