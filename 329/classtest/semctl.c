#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
int main(){
	int semid;
	semid=semget((key_t)666,5,IPC_CREAT|0600);
	if(semid<0){
		perror("semget");
		return -1;
	}
	int ret;
	unsigned short array[]={2,1,3,4,5};
	ret=semctl(semid,0,SETALL,array);
	if(0!=ret){
		perror("semctl1");
	}
	int arrayBuf[5]={0};
	ret=semctl(semid,0,GETALL,(unsigned short*)arrayBuf);
	if(0!=ret){
		perror("semctl2");
	}
	printf("%u\n",arrayBuf[0]);
	return 0;
}
