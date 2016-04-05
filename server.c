#include "fun.h"
#define CS 10
void nonblock(int *fd){
	int stats;
	stats=fcntl(*fd,F_GETFL);
	stats=stats|O_NONBLOCK;
	fcntl(*fd,F_SETFL,stats);
}
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args\n");
		return -1;
	}
	int sfd;
	int ret,ret1;
	int flag=1;
	sfd=socket(AF_INET,SOCK_DGRAM,0);
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	nonblock(&sfd);
	struct sockaddr_in nad;
	nad.sin_family=AF_INET;
	nad.sin_port=htons(atoi(argv[2]));
	nad.sin_addr.s_addr=inet_addr(argv[1]);
	ret=bind(sfd,(struct sockaddr*)&nad,16);
	if(-1==ret){
		perror("bind");
		return -1;
	}
	struct sockaddr_in curc;
	bzero(&curc,sizeof(curc));
	int curl;
	char buf[BL];
	int efd;
	efd=epoll_create(1);
	if(-1==efd){
		perror("epoll_create");
		return -1;
	}
	struct epoll_event ev1,ev2,ev[2];
	ev1.events=EPOLLIN;
	ev2.events=EPOLLIN;
	ev1.data.fd=sfd;
	ev2.data.fd=0;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev1);
	epoll_ctl(efd,EPOLL_CTL_ADD,0,&ev2);
	int i;
	while(1){
		bzero(ev,sizeof(ev));
		ret=epoll_wait(efd,ev,2,-1);
		for(i=0;i<ret;i++){
			if(ev[i].events==EPOLLIN&&ev[i].data.fd==0){
				bzero(buf,sizeof(buf));
				ret1=read(0,buf,sizeof(buf));
				if(-1==ret){
					perror("read");
					return -1;
				}
				ret1=sendto(sfd,buf,strlen(buf),0,(struct sockaddr*)&curc,curl);
				if(-1==ret){
					perror("sendto");
					return -1;
				}
			}
			if(ev[i].events==EPOLLIN&&ev[i].data.fd==sfd){
				if(flag==0){
					while(1){
  					bzero(buf,sizeof(buf));
  					ret1=recvfrom(sfd,buf,sizeof(buf),0,(struct sockaddr*)&curc,&curl);
						if(ret1>0){
  						printf("%s",buf);
						}else{
							break;
						}
					}
				}else{
					curl=sizeof(struct sockaddr);
					printf("curl is %d\n",curl);
					ret1=recvfrom(sfd,buf,sizeof(buf),0,(struct sockaddr*)&curc,&curl);
					if(-1==ret){
						return -1;
					}
					flag=0;
					printf("curaddr is %s\ncur port is %d\n",inet_ntoa(curc.sin_addr),ntohs(curc.sin_port));
				}
			}
		}
	}
}
