//���ļ��������ļ�Ϊ��չ�ļ�
#include "externFun.h"

//������ִ󲿷��豸���Ӳ��ϣ�������Ӧ���ǳ����Ǵ˴�
//ID������ȷ����1�����Ȳ���ȷ����0
int StringSub(char *pSstr,char *pDstr,int subLen)
{
	int i = 0;
	while((pSstr[i] != '\0') 
		&& (pSstr[i] != '\r') 
		&& (pSstr[i] != ',') 
		&& (pSstr[i] != '>')
		&& (i<(subLen-1))){
		pDstr[i] = pSstr[i];			
		i ++;
	}

	if (i == (subLen-1))
		return 1;
	else 
		return 0;
}

//SIGKILL����źŲ�ע��
//���������signal����ע�ᵽ�ں��У�
//�����쳣ʱ�����ں˻ص�����
//������ֻ����־��ӡ����
//����ע�����쳣���������쳣����ʱ�����ᵼ�³����˳�

void ExceptionSIGHUP(int ex)
{
	WriteLog("SIGHUP!");
	sleep(1);//����д�����
}

void ExceptionSIGINT(int ex)
{
	WriteLog("SIGINT!");
	sleep(1);//����д�����
}

void ExceptionSIGQUIT(int ex)
{
	WriteLog("SIGQUIT!");
	sleep(1);//����д�����
}

void ExceptionSIGILL(int ex)
{
	WriteLog("SIGILL!");
	sleep(1);//����д�����
}

void ExceptionSIGTRAP(int ex)
{
	WriteLog("SIGTRAP!");
	sleep(1);//����д�����
}

void ExceptionSIGABRT(int ex)
{
	WriteLog("SIGABRT!");
	sleep(1);//����д�����
}

void ExceptionSIGBUS(int ex)
{
	WriteLog("SIGBUS!");
	sleep(1);//����д�����
}

void ExceptionSIGFPE(int ex)
{
	WriteLog("SIGFPE!");
	sleep(1);//����д�����
}

void ExceptionSIGUSR1(int ex)
{
	WriteLog("SIGUSR1!");
	sleep(1);//����д�����
}

void ExceptionSIGSEGV(int ex)
{
	WriteLog("SIGSEGV!");
	sleep(1);//����д�����
}

void ExceptionSIGUSR2(int ex)
{
	WriteLog("SIGUSR2!");
	sleep(1);//����д�����
}

void ExceptionSIGPIPE(int ex)
{
	WriteLog("SIGPIPE!");
	sleep(1);//����д�����
}

void ExceptionSIGALRM(int ex)
{
	WriteLog("SIGALRM!");
	sleep(1);//����д�����
}

void ExceptionSIGTERM(int ex)
{
	WriteLog("SIGTERM!");
	sleep(1);//����д�����
}

void ExceptionSIGSTKFLT(int ex)
{
	WriteLog("SIGSTKFLT!");
	sleep(1);//����д�����
}

void ExceptionSIGCHLD(int ex)
{
	WriteLog("SIGCHLD!");
	sleep(1);//����д�����
}

void ExceptionSIGCONT(int ex)
{
	WriteLog("SIGCONT!");
	sleep(1);//����д�����
}

void ExceptionSIGSTOP(int ex)
{
	WriteLog("SIGSTOP!");
	sleep(1);//����д�����
}

void ExceptionSIGTSTP(int ex)
{
	WriteLog("SIGTSTP!");
	sleep(1);//����д�����
}

void ExceptionSIGTTIN(int ex)
{
	WriteLog("SIGTTIN!");
	sleep(1);//����д�����
}

void ExceptionSIGTTOU(int ex)
{
	WriteLog("SIGTTOU!");
	sleep(1);//����д�����
}

void ExceptionSIGURG(int ex)
{
	WriteLog("SIGURG!");
	sleep(1);//����д�����
}

void ExceptionSIGXCPU(int ex)
{
	WriteLog("SIGXCPU!");
	sleep(1);//����д�����
}

void ExceptionSIGXFSZ(int ex)
{
	WriteLog("SIGXFSZ!");
	sleep(1);//����д�����
}

void ExceptionSIGVTALRM(int ex)
{
	WriteLog("SIGVTALRM!");
	sleep(1);//����д�����
}

void ExceptionSIGPROF(int ex)
{
	WriteLog("SIGPROF!");
	sleep(1);//����д�����
}

void ExceptionSIGWINCH(int ex)
{
	WriteLog("SIGWINCH!");
	sleep(1);//����д�����
}

void ExceptionSIGIO(int ex)
{
	WriteLog("SIGIO!");
	sleep(1);//����д�����
}

void ExceptionSIGPWR(int ex)
{
	WriteLog("SIGPWR!");
	sleep(1);//����д�����
}

void ExceptionSIGSYS(int ex)
{
	WriteLog("SIGSYS!");
	sleep(1);//����д�����
}


