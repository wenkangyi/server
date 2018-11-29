#include "includes.h"

pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;


//写日志函数，可重入
//s为要记录的字符串
void WriteLog(char *s)
{
	time_t t;
    struct tm *locp = NULL;
    struct tm loc;
	char filename[50];
	int i = 0;
	FILE *fd;

	#if SYS_DEBUG
	printf("%s\r\n",s);
	#endif
	
	t = time(NULL);
	locp = localtime(&t);
	if (locp != NULL)
	{
		loc = *locp;
		for(i=0;i<50;i++)
			filename[i] = 0;
		
		sprintf( filename, "/home/Applog/App%d-%d-%d", (loc.tm_year+1900), (loc.tm_mon+1), loc.tm_mday);
		pthread_mutex_lock(&fileMutex);	
		fd = fopen(filename,"a+");
		fprintf (fd, "%d:%d:%d %s\n",loc.tm_hour,loc.tm_min,loc.tm_sec,s);
		fclose(fd);
		pthread_mutex_unlock(&fileMutex);
	}	
}




