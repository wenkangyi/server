//此文件包含的文件为扩展文件
#include "externFun.h"

//如果出现大部分设备连接不上，即问题应该是出现是此处
//ID长度正确返回1，长度不正确返回0
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

//SIGKILL这个信号不注册
//这个函数由signal函数注册到内核中，
//产生异常时，由内核回调处理
//在这里只做日志打印处理
//由于注册了异常处理，所以异常产生时，不会导致程序退出

void ExceptionSIGHUP(int ex)
{
	WriteLog("SIGHUP!");
	sleep(1);//避免写入过快
}

void ExceptionSIGINT(int ex)
{
	WriteLog("SIGINT!");
	sleep(1);//避免写入过快
}

void ExceptionSIGQUIT(int ex)
{
	WriteLog("SIGQUIT!");
	sleep(1);//避免写入过快
}

void ExceptionSIGILL(int ex)
{
	WriteLog("SIGILL!");
	sleep(1);//避免写入过快
}

void ExceptionSIGTRAP(int ex)
{
	WriteLog("SIGTRAP!");
	sleep(1);//避免写入过快
}

void ExceptionSIGABRT(int ex)
{
	WriteLog("SIGABRT!");
	sleep(1);//避免写入过快
}

void ExceptionSIGBUS(int ex)
{
	WriteLog("SIGBUS!");
	sleep(1);//避免写入过快
}

void ExceptionSIGFPE(int ex)
{
	WriteLog("SIGFPE!");
	sleep(1);//避免写入过快
}

void ExceptionSIGUSR1(int ex)
{
	WriteLog("SIGUSR1!");
	sleep(1);//避免写入过快
}

void ExceptionSIGSEGV(int ex)
{
	WriteLog("SIGSEGV!");
	sleep(1);//避免写入过快
}

void ExceptionSIGUSR2(int ex)
{
	WriteLog("SIGUSR2!");
	sleep(1);//避免写入过快
}

void ExceptionSIGPIPE(int ex)
{
	WriteLog("SIGPIPE!");
	sleep(1);//避免写入过快
}

void ExceptionSIGALRM(int ex)
{
	WriteLog("SIGALRM!");
	sleep(1);//避免写入过快
}

void ExceptionSIGTERM(int ex)
{
	WriteLog("SIGTERM!");
	sleep(1);//避免写入过快
}

void ExceptionSIGSTKFLT(int ex)
{
	WriteLog("SIGSTKFLT!");
	sleep(1);//避免写入过快
}

void ExceptionSIGCHLD(int ex)
{
	WriteLog("SIGCHLD!");
	sleep(1);//避免写入过快
}

void ExceptionSIGCONT(int ex)
{
	WriteLog("SIGCONT!");
	sleep(1);//避免写入过快
}

void ExceptionSIGSTOP(int ex)
{
	WriteLog("SIGSTOP!");
	sleep(1);//避免写入过快
}

void ExceptionSIGTSTP(int ex)
{
	WriteLog("SIGTSTP!");
	sleep(1);//避免写入过快
}

void ExceptionSIGTTIN(int ex)
{
	WriteLog("SIGTTIN!");
	sleep(1);//避免写入过快
}

void ExceptionSIGTTOU(int ex)
{
	WriteLog("SIGTTOU!");
	sleep(1);//避免写入过快
}

void ExceptionSIGURG(int ex)
{
	WriteLog("SIGURG!");
	sleep(1);//避免写入过快
}

void ExceptionSIGXCPU(int ex)
{
	WriteLog("SIGXCPU!");
	sleep(1);//避免写入过快
}

void ExceptionSIGXFSZ(int ex)
{
	WriteLog("SIGXFSZ!");
	sleep(1);//避免写入过快
}

void ExceptionSIGVTALRM(int ex)
{
	WriteLog("SIGVTALRM!");
	sleep(1);//避免写入过快
}

void ExceptionSIGPROF(int ex)
{
	WriteLog("SIGPROF!");
	sleep(1);//避免写入过快
}

void ExceptionSIGWINCH(int ex)
{
	WriteLog("SIGWINCH!");
	sleep(1);//避免写入过快
}

void ExceptionSIGIO(int ex)
{
	WriteLog("SIGIO!");
	sleep(1);//避免写入过快
}

void ExceptionSIGPWR(int ex)
{
	WriteLog("SIGPWR!");
	sleep(1);//避免写入过快
}

void ExceptionSIGSYS(int ex)
{
	WriteLog("SIGSYS!");
	sleep(1);//避免写入过快
}


