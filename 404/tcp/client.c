#include "fun.h"
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args");
		return -1;
	}
	int ufd;
	ufd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==ufd){
		perror("socket");
		return -1;
	}
	struct sockaddr_in ad;
	bzero(&ad,sizeof(struct sockaddr));
	ad.sin_family=AF_INET;
	ad.sin_port=htons(atoi(argv[2]));
	ad.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=connect(ufd,(struct sockaddr*)&ad,sizeof(struct sockaddr));
	if(-1==ret){
		perror("connect");
		return -1;
	}
	fd_set sw;
	char buf[BL];
	while(1){
		FD_ZERO(&sw);
		FD_SET(0,&sw);
		FD_SET(ufd,&sw);
		select(ufd+1,&sw,NULL,NULL,NULL);
		if(FD_ISSET(ufd,&sw)){
			bzero(buf,sizeof(buf));
			ret=recv(ufd,buf,sizeof(buf),0);
			if(-1==ret){
				perror("recv");
				return -1;
			}else if(0==ret){
				close(ufd);
				return -1;
			}else{
				puts(buf);
			}
		}
		if(FD_ISSET(0,&sw)){
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
	}
}
