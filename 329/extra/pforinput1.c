#include "fun.h"
int main(int argc,char *argv[]){
	//使用全局变量而在程序执行过程中进行赋值的方式来记录对方的pid，这样当
	//当前进程结束后，就可以使用kill(0,9)和kill(pid,9)来回收双方资源
	//这里实现的只是自杀的方式
	signal(SIGINT,handle);
	//先实现共享内存
	if(4!=argc){
		printf("error args\n");
		return -1;
	}
	key_t key;
	key=ftok(argv[1],1);
	if(-1==key){
		perror("ftok");
		return -1;
	}
	int shmid;
	shmid=shmget(key,1<<10,IPC_CREAT|0600);
	if(-1==shmid){
		perror("shmget");
		return -1;
	}
	pchat c;
	c=(pchat)shmat(shmid,NULL,0);
	if((pchat)-1==c){
		perror("shmat");
		return -1;
	}
	//独占的共享内存因此需要设置信号量
	int semid;
	semid=semget((key_t)1001,1,IPC_CREAT|0600);
	semctl(semid,0,SETVAL,1);
	struct sembuf p,v;//num,op,flg
	p.sem_num=0;
	p.sem_op=-1;
	p.sem_flg=SEM_UNDO;//一般都用这个
	v.sem_num=0;
	v.sem_op=1;
	v.sem_flg=SEM_UNDO;
	//管道通信连接管道
	int fdpr,fdpw;
	fdpr=open(argv[2],O_RDONLY);
	if(-1==fdpr){
		perror("openr");
		return -1;
	}
	fdpw=open(argv[3],O_WRONLY);
	if(-1==fdpr){
		perror("openw");
		return -1;
	}
	//等待标准输入和可读
	//int select(int maxfd, fd_set *readset,fd_set *writeset,
	//fd_set *exceptionset, const struct timeval * timeout);
	fd_set s;
	char buf[L];
	int ret;
	while(1){
		FD_ZERO(&s);
		FD_SET(0,&s);
		FD_SET(fdpr,&s);
		ret=select(fdpr+1,&s,NULL,NULL,NULL);
		if(-1==ret){
			perror("select");
			return -1;
		}
		if(FD_ISSET(0,&s)){
		//标准输入，应该写入管道，并写入共享内存中，标记为自己的信息1
			memset(buf,0,sizeof(buf));
			read(0,buf,sizeof(buf));
			ret=write(fdpw,buf,strlen(buf)-1);
			if(ret<=0){
				perror("write");
				return -1;
			}
			semop(semid,&p,1);
			if(0==c->isnew){
				strncpy(c->buf,buf,strlen(buf)-1);
				c->isnew=1;
			}
			semop(semid,&v,1);
		}
		if(FD_ISSET(fdpr,&s)){
		//管道可读，应该写到共享内存中，标记为别人的信息2
			memset(buf,0,sizeof(buf));
			ret=read(fdpr,buf,sizeof(buf));
			if(ret<=0){
				perror("read");
				return -1;
			}
			semop(semid,&p,1);//真正需要的时候才上锁
			if(0==c->isnew){
				strncpy(c->buf,buf,strlen(buf));
				c->isnew=2;
			}
			semop(semid,&v,1);
		}
	}
}
