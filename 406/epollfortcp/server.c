#include "fun.h"
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	int ret;
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(atoi(argv[2]));
	ad.sin_addr.s_addr=inet_addr(argv[1]);
	ret=bind(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1;
	}
	ret=listen(sfd,10);
	if(-1==ret){
		perror("listen");
		return -1;
	}
	int ufd=-1;
	int efd;
	efd=epoll_create(1);
	if(-1==efd){
		perror("epoll_create");
		return -1;
	}
	struct epoll_event ev,evs[3];
	bzero(evs,sizeof(evs));
	ev.data.fd=0;
	ev.events=EPOLLIN;
	ret=epoll_ctl(efd,EPOLL_CTL_ADD,0,&ev);
	if(-1==ret){
		perror("epoll_ctl");
		return -1;
	}
	ev.data.fd=sfd;
	ev.events=EPOLLIN;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	if(-1==ret){
		perror("epoll_ctl");
		return -1;
	}
	char buf[BL]={0};
	char buf1[3]={0};
	int retforw,i;
	struct sockaddr_in caddr;
	int caddrlen=sizeof(struct sockaddr);
	while(1){
		bzero(evs,sizeof(evs));
		retforw=epoll_wait(efd,evs,3,-1);
		if(-1==retforw){
			perror("epoll_wait");
			return -1;
		}
		for(i=0;i<retforw;i++){
			if(evs[i].events==EPOLLIN&&evs[i].data.fd==sfd){
				ufd=accept(sfd,(struct sockaddr*)&caddr,&caddrlen);
				if(-1==ufd){
					perror("accept");
					return -1;
				}
				printf("client linked\naddr:%s\nport:%d\n",inet_ntoa(caddr.sin_addr),ntohs(caddr.sin_port));
				ev.data.fd=ufd;
				ev.events=EPOLLIN|EPOLLET;
				ret=epoll_ctl(efd,EPOLL_CTL_ADD,ufd,&ev);
				if(-1==ret){
					perror("epoll_ctl");
					return -1;
				}
				nonblock(&ufd);
			}
			if(evs[i].events==EPOLLIN&&evs[i].data.fd==0){
				bzero(buf,sizeof(buf));
				ret=read(0,buf,sizeof(buf));
				if(-1==ret){
					perror("read");
					return -1;
				}
				ret=send(ufd,buf,strlen(buf)-1,0);
				if(-1==ret){
					perror("send");
					return -1;
				}
			}
			if(evs[i].events==EPOLLIN|EPOLLET&&evs[i].data.fd==ufd){
				while(1){
					bzero(buf1,sizeof(buf1));
					ret=recv(ufd,buf1,sizeof(buf1)-1,0);
					if(-1==ret){
						printf("\n");
						break;
					}else if(0==ret){
						ev.data.fd=ufd;
						ret=epoll_ctl(efd,EPOLL_CTL_DEL,ufd,&ev);
						if(-1==ret){
							perror("epoll_ctl");
							return -1;
						}
						close(ufd);
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
