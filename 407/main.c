#include "fun.h"
int main(int argc,char *argv[]){
	//确定参数正确
	if(4!=argc){
		printf("error args\n");
		return -1;
	}
	int num=atoi(argv[3]);
	pchild childs;
	childs=(pchild)malloc(num*sizeof(child));
	makechild(childs,num);
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(atoi(argv[2]));
	ad.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1;
	}
	ret=listen(sfd,num);
	int efd;
	efd=epoll_create(1);
	if(-1==efd){
		perror("epoll_create");
		return -1;
	}
	struct epoll_event ev,*evs;
	evs=(struct epoll_event*)malloc((num+1)*sizeof(struct epoll_event));
	ev.events=EPOLLIN;
	ev.data.fd=sfd;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	int i;
	for(i=0;i<num;i++){
		ev.data.fd=childs[i].fd;
		epoll_ctl(efd,EPOLL_CTL_ADD,childs[i].fd,&ev);
	}
	int retforw;
	int ufd;
	int j;
	int flag;
	while(1){
		retforw=epoll_wait(efd,evs,num+1,-1);
		if(-1==retforw){
			perror("epoll_wait");
			return -1;
		}
		for(i=0;i<retforw;i++){
			if(EPOLLIN==evs[i].events&&evs[i].data.fd==sfd){//没有监控标准输入，第一个条件其实可以省略
				ufd=accept(sfd,NULL,NULL);
				if(-1==ufd){
					perror("accept");
					return -1;
				}
				for(j=0;j<num;j++){
					if(0==childs[j].busy)break;
				}
				if(j==num){
					printf("system busy,try again later\n");
					continue;
				}
				ret=sendfd(childs[j].fd,ufd);
				if(-1==ret){
					printf("sendfd failed");
					return -1;
				}
				childs[j].busy=1;
			}
			for(j=0;j<num;j++){
				if(EPOLLIN==evs[i].events&&evs[i].data.fd==childs[j].fd){
					ret=read(childs[j].fd,&flag,sizeof(int));
					if(-1==ret){
						perror("process status is not normal,you may restart");
					}
					childs[j].busy=0;
				}
			}
		}

	}
	wait(NULL);
	return 0;
}
