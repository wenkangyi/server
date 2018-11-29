#ifndef _EXTERN_FUN_H__
#define _EXTERN_FUN_H__

#include "includes.h"


extern int StringSub(char *pSstr,char *pDstr,int subLen);
extern void ExceptionSIGHUP(int ex);
extern void ExceptionSIGINT(int ex);
extern void ExceptionSIGQUIT(int ex);
extern void ExceptionSIGILL(int ex);
extern void ExceptionSIGTRAP(int ex);
extern void ExceptionSIGABRT(int ex);
extern void ExceptionSIGBUS(int ex);
extern void ExceptionSIGFPE(int ex);
extern void ExceptionSIGUSR1(int ex);
extern void ExceptionSIGSEGV(int ex);
extern void ExceptionSIGUSR2(int ex);
extern void ExceptionSIGPIPE(int ex);
extern void ExceptionSIGALRM(int ex);
extern void ExceptionSIGTERM(int ex);
extern void ExceptionSIGSTKFLT(int ex);
extern void ExceptionSIGCHLD(int ex);
extern void ExceptionSIGCONT(int ex);
extern void ExceptionSIGSTOP(int ex);
extern void ExceptionSIGTSTP(int ex);
extern void ExceptionSIGTTIN(int ex);
extern void ExceptionSIGTTOU(int ex);
extern void ExceptionSIGURG(int ex);
extern void ExceptionSIGXCPU(int ex);
extern void ExceptionSIGXFSZ(int ex);
extern void ExceptionSIGVTALRM(int ex);
extern void ExceptionSIGPROF(int ex);
extern void ExceptionSIGWINCH(int ex);
extern void ExceptionSIGIO(int ex);
extern void ExceptionSIGPWR(int ex);
extern void ExceptionSIGSYS(int ex);



#endif

