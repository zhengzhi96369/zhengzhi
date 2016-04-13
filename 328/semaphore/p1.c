#include "fun.h"
int main(int argc,char *argv[]){
	if(argc!=2){
		printf("error args!\n");
		return -1;
	}
	key_t key;
	key=ftok(argv[1],1);
	int shmid;
	shmid=shmget(key,1<<10,IPC_CREAT|0600);
	if(-1==shmid){
		perror("shmid");
		return -1;
	}
	pchat pc;
	pc=(pchat)shmat(shmid,NULL,0);
	if((pchat)-1==pc){
		perror("shmat");
		return -1;
	}
	int semid;
	semid=semget((key_t)1234,1,0600|O_CREAT);
	semctl(semid,0,SETVAL,0);
	struct sembuf p,v; 
	p.sem_num=0;
	p.sem_op=-1;
	p.sem_flg=SEM_UNDO;	
	v.sem_num=0;
	v.sem_op=1;
	v.sem_flg=SEM_UNDO;
	int stat;
	stat=fcntl(0,F_GETFL);
	stat=stat|O_NONBLOCK;
	fcntl(0,F_SETFL,O_NONBLOCK);
	char buf[1024];
	int ret;
	while(1){
		memset(buf,0,sizeof(buf));
		ret=read(0,buf,sizeof(buf));
		if(ret>0){
			semop(semid,&p,1);
			if(0==pc->read){
				strncpy(pc->buf,buf,strlen(buf)-1);
				pc->read=1;
			}
			semop(semid,&v,1);
		}
		semop(semid,&p,1);
		if(2==pc->read){
			puts(pc->buf);
			memset(pc->buf,0,sizeof(pc->buf));//之前写错很多处，这种东西应该用宏定义指定，无伤大雅
			pc->read=0;
		}
		semop(semid,&v,1);
	}
}
