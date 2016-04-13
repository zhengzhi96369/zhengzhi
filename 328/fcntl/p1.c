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
		perror("creat failed");
		return -1;
	}
	pchat p;
	p=(pchat)shmat(shmid,NULL,0);
	if((pchat)-1==p){
		perror("shmat");
		return -1;
	}
	p->read=0;
	char buf[1024];
	int ret;
	int stat;
	stat=fcntl(0,F_GETFL);
	fcntl(0,F_SETFL,stat|O_NONBLOCK);
	while(1){
		memset(buf,0,sizeof(buf));
		ret=read(0,buf,sizeof(buf));
		if(ret>0){
			//此时写，有了标准输入才写
			lock(p);
			if(0==p->read){
				strncpy(p->buf,buf,strlen(buf)-1);
				p->read=1;
			}
			unlock(p);
		}
			//此时读，每次都尝试读
		lock(p);
		if(2==p->read){
			puts(p->buf);
			p->read=0;
			memset(p->buf,0,sizeof(buf));
		}
		unlock(p);
	}
}
