#include "includes.h"








//设备终端接收缓冲池
BUFFER_POOL2 devBufferPool[DEV_BUFFER_POOL_MAX_VAL];
//软件终端接收缓冲池
BUFFER_POOL2 softBufferPool[SOFT_BUFFER_POOL_MAX_VAL];

//初始化缓冲池
//这里对第一次的动态缓冲池进行了重构，
//不再使用动态内存
void InitBufferPool2()
{
	int i =0;
	int k = 0;
	for(i=0;i<DEV_BUFFER_POOL_MAX_VAL;i++){
		devBufferPool[i].readIndex = 0;
		devBufferPool[i].writeIndex= 0;
		for(k<0;k<65536;k++){
			devBufferPool[i].bpNode[k].cmdLen = 0;
			devBufferPool[i].bpNode[k].sockfd = 0;
			devBufferPool[i].bpNode[k].useFlag = 0;
			memset((void*)devBufferPool[i].bpNode[k].cmd,0,CMD_MAX_LEN);
		}
	}

	for(i=0;i<SOFT_BUFFER_POOL_MAX_VAL;i++){
		softBufferPool[i].readIndex = 0;
		softBufferPool[i].writeIndex= 0;
		for(k<0;k<65536;k++){
			softBufferPool[i].bpNode[k].cmdLen = 0;
			softBufferPool[i].bpNode[k].sockfd = 0;
			softBufferPool[i].bpNode[k].useFlag = 0;
			memset((void*)softBufferPool[i].bpNode[k].cmd,0,CMD_MAX_LEN);
		}
	}		
		

	
}






