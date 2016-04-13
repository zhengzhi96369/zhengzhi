#include "fun.h"
int main(int argc,char *argv[]){
	if(2!=argc){
		printf("error args!\n");
		return -1;
	}
	//链接共享内存
	key_t key;
	key=ftok(argv[1],1);
	int shmid;
	shmid=shmget(key,1<<10,IPC_CREAT|0600);
	pchat c;
	c=shmat(shmid,NULL,0);
	if((pchat)-1==c){
		perror("shmat");
		return -1;
	}
	int semid;
	semid=semget((key_t)1001,1,IPC_CREAT|0600);
	semctl(semid,0,SETVAL,1);
	struct sembuf p,v;
	p.sem_num=0;
	p.sem_op=-1;
	p.sem_flg=SEM_UNDO;
	v.sem_num=0;
	v.sem_op=1;
	v.sem_flg=SEM_UNDO;
	char buf[L];
	while(1){
		semop(semid,&p,1);
		memset(buf,0,sizeof(buf));
		if(1==c->isnew){
			strcpy(buf,c->buf);
			printf("I:\n");
			puts(buf);
			memset(c->buf,0,L);
			c->isnew=0;
		}else if(2==c->isnew){
			strcpy(buf,c->buf);
			printf("H/S:\n");
			puts(buf);
			memset(c->buf,0,L);
			c->isnew=0;
		}
		semop(semid,&v,1);
	}
}
