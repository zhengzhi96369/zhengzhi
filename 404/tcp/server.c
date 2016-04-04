#include "fun.h"
int main(int argc,char *argv[]){
	//传入的两个参数分别对应服务器ip和所用的端口号
	if(3!=argc){
		printf("error args!\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	struct sockaddr_in ad,cl;
	int ret;
	bzero(&ad,sizeof(struct sockaddr_in));
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
	char buf[BL];
	fd_set sw;//需要等待的
	fd_set cs;//客户端的数量
	int ufd=-1;
	int maxfd;
	while(1){
		FD_ZERO(&sw);
		memcpy(&sw,&cs,sizeof(fd_set));
		FD_SET(0,&sw);
		FD_SET(sfd,&sw);
		if(FD_ISSET(ufd,&cs)){
			maxfd=ufd+1;
		}else{
			maxfd=sfd+1;
		}
		ret=select(maxfd,&sw,NULL,NULL,NULL);
		if(ret>0){
			if(FD_ISSET(sfd,&sw)){
				int len=sizeof(struct sockaddr);
				ufd=accept(sfd,(struct sockaddr*)&cl,&len);
				if(-1==ufd){
					perror("accept");
					return -1;
				}
				printf("client ip=%s\nclient port=%d\n",inet_ntoa(cl.sin_addr),htons(cl.sin_port));
				FD_SET(ufd,&cs);
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
			if(FD_ISSET(ufd,&sw)){
				bzero(buf,sizeof(buf));
				ret=recv(ufd,buf,sizeof(buf),0);
				if(-1==ret){
					perror("recv");
					return -1;
				}else if(0==ret){
					FD_CLR(ufd,&cs);
					close(ufd);
				}else{
					puts(buf);
				}
			}
		}else{
			perror("select");
			return -1;
		}
	}
	close(sfd);
}
