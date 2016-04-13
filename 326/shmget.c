#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main(int argc,char *argv[]){
	if(2!=argc){
		printf("error args!\n");
		exit(-1);
	}
	key_t key;
	key=ftok(argv[1],1);
	int shmid;
	shmid=shmget(key,1<<10,IPC_EXCL|IPC_CREAT|0666);
	if(shmid==-1){
		perror("shmget failed");
		return -1;
	}else{
		printf("new shmid:%d\n",shmid);
	}
	return 0;
}
