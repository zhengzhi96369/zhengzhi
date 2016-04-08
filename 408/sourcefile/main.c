#include "factory.h"
int send_n(int fd,char *start,int len){
	int total=0;
	int one;
	while(total<len){
		one=send(fd,start+total,len-total,0);
		if(-1==one){
			perror("send");
			return -1;
		}
		total=total+one;
	}
	return 0;
}
int send_file(fd){
	int fdr;
	fdr=open(F,O_RDONLY);
	if(-1==fdr){
		perror("file open");
		return -1;
	}
	int ret;
	data d;
	d.len=strlen(F);
	bzero(d.buf,sizeof(d.buf));
	strcpy(d.buf,F);
	ret=send_n(fd,(char*)&d,d.len+4);
	if(-1==ret){
		printf("file send error\n");
		return -1;
	}
	while(1){
		d.len=read(fdr,d.buf,sizeof(d.buf));
		if(-1==d.len){
			perror("read");
			return -1;
		}else if(0==d.len){
			break;
		}else{
			send_n(fd,(char*)&d,d.len+4);
		}
	}
	int flag=0;
	send_n(fd,(char*)&flag,sizeof(int));
	return 0;
}
void* func(void* p){
	//接收文件描述符
	//发文件
	pfac f=(pfac)p;
	int fd;
	int ret;
	while(1){
		fd=assign(f);
		ret=send_file(fd);
		if(-1==ret){
			exit(-1);
		}
	}
}
int main(int argc,char *argv[]){//ip port pnum capability
	if(5!=argc){
		printf("error args\n");
		return -1;
	}
	int capability=atoi(argv[4]);
	int pnum=atoi(argv[3]);
	factory f;
	fac_init(&f,pnum,capability,func);
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
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
	ret=listen(sfd,capability);
	if(-1==ret){
		perror("listen");
		return -1;
	}
	int efd;
	efd=epoll_create(1);
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=sfd;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	pnode n;
	while(1){
		ret=epoll_wait(efd,&ev,1,-1);
		if(ret>0){
			if(ev.events==EPOLLIN&&ev.data.fd==sfd){
				n=(pnode)malloc(sizeof(node));
				n->fd=accept(sfd,NULL,NULL);
				if(n->fd<0){
					perror("accept");
					return -1;
				}
				n->next=NULL;
				visit(&f,n);
			}	
		}
	}
}
