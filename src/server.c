#include "includes.h"



int main(void)
{

	pthread_t t1,t2;
	void *res = NULL;
	int s;

	
	sleep(10);//ÑÓÊ±20SÆô¶¯
	
	WriteLog("System Start!");
	
	SystemParamInit();
		
	s = pthread_create(&t1, NULL, AcceptThread, (void*)"Hello world\n");
	if (s != 0)
	{
		WriteLog("pthread_create->AcceptThread Error");
		perror( "pthread_create");
	}
	else
	{
		WriteLog("pthread_create->AcceptThread successful");
	}
	  

	#if ADD_SOFT_COMM
	s = pthread_create(&t2, NULL, AcceptSoftThread, (void*)"Hello world\n");
	if (s != 0)
	{
		WriteLog("pthread_create->AcceptSoftThread Error");
		perror( "pthread_create");
	}
	else{
		WriteLog("pthread_create->AcceptSoftThread successful");
	}
	#endif

	
	s = pthread_join(t1, &res);
	if (s != 0)
		perror( "pthread_join");

	WriteLog("APP End!");
	


    /*while (1) {
        n = read(cfd, buf, sizeof(buf));
        for (i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);
        write(cfd, buf, n);
    }

    close(lfd);
    close(cfd);*/

    return 0;
}

