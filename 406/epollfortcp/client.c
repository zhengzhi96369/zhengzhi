#include "fun.h"
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args\n");
		return -1;
	}
	int ret;
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd){
		perror("sfd");
		return -1;
	}
	nonblock(&sfd);
	struct sockaddr_in saddr;
	int saddrlen=sizeof(struct sockaddr);
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(atoi(argv[2]));
	saddr.sin_addr.s_addr=inet_addr((argv[1]));
	ret=connect(sfd,(struct sockaddr*)&saddr,saddrlen);
//	if(-1==ret){
//		perror("connect");
//		return -1;
//	}
	int efd;
	efd=epoll_create(1);
	if(-1==efd){
		perror("epoll create");
		return -1;
	}
	struct epoll_event ev,evs[2];
	bzero(evs,sizeof(evs));
	ev.data.fd=0;
	ev.events=EPOLLIN;
	ret=epoll_ctl(efd,EPOLL_CTL_ADD,0,&ev);
	if(-1==ret){
		perror("epoll_ctl");
		return -1;
	}
	ev.data.fd=sfd;
	ev.events=EPOLLIN|EPOLLET;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	char buf[BL]={0};
	char buf1[3]={0};
	int retforw,i;
	while(1){
		bzero(evs,sizeof(evs));
		retforw=epoll_wait(efd,evs,2,-1);
		if(-1==retforw){
			perror("epoll_wait");
			return -1;
		}
		for(i=0;i<retforw;i++){
			if(evs[i].events==EPOLLIN&&evs[i].data.fd==0){
				bzero(buf,sizeof(buf));
				ret=read(0,buf,sizeof(buf));
				if(-1==ret){
					perror("read");
					return -1;
				}
				ret=send(sfd,buf,strlen(buf)-1,0);
				if(-1==ret){
					perror("send");
					return -1;
				}
			}
			if(evs[i].events==EPOLLIN|EPOLLET&&evs[i].data.fd==sfd){
				while(1){
					bzero(buf1,sizeof(buf1));
					ret=recv(sfd,buf1,sizeof(buf1)-1,0);
					if(-1==ret){
						printf("\n");
						break;
					}else if(0==ret){
						close(sfd);
						close(efd);
						return 0;
					}else{
						printf("%s",buf1);
					}
				}			
			}
		}
	}
	close(sfd);
	close(efd);
	return 0;
}
