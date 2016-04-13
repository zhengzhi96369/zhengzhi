#include "factory.h"
#include "command.h"
int send_n(int fd,char *start,int len){
	int total=0;
	int one;
	while(total<len){
		one=send(fd,start+total,len-total,0);
		if(-1==one){
			printf("send error");
			return -1;
		}
		total=total+one;
	}
	return 0;
}
int recv_n(int fd,char *start,int len){
	int total=0;
	int one;
	while(total<len){
		one=recv(fd,start+total,len-total,0);
		if(-1==one){
			printf("recv error");
			return -1;
		}
		total=total+one;
	}
	return 0;
}
int send_c(int fd,n_cmd* command){
	int len=com_getlen(command);
	int ret;
	ret=send_n(fd,(char*)&len,sizeof(int));
	if(-1==ret){
		return -1;
	}
	ret=send_n(fd,(char*)command,len);
	if(-1==ret){
		return -1;
	}
	return 0;
}
int send_file(int fd,char *path){
	data d;
	int ret;
	int fdr=open(path,O_RDONLY);
	if(-1==fdr){
		return -1;
	}
	while(1){
		bzero(d.buf,sizeof(d.buf));
		ret=read(fdr,d.buf,sizeof(d.buf)-1);
		if(-1==ret){
			printf("send error\n");
			return -1;
		}else if(0==ret){
			break;
		}else{
			d.len=ret;
			send_n(fd,(char*)&d,d.len+4);
		}
	}
	ret=0;
	send_n(fd,(char*)&ret,sizeof(int));
	return 0;
}
int main(int argc,char *argv[]){
	if(4!=argc){
		printf("error args\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(atoi(argv[2]));
	ad.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=connect(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr));
	if(-1==ret){
		perror("connect");
		return -1;
	}
	cset cs;
	cs.head=NULL;
	ret=com_init(argv[3],&cs);
	if(-1==ret){
		printf("command set init failed\n");
		return -1;
	}
	int efd;
	efd=epoll_create(1);
	struct epoll_event ev,evs[2];
	bzero(evs,sizeof(evs));
	ev.data.fd=0;
	ev.events=EPOLLIN;
	epoll_ctl(efd,EPOLL_CTL_ADD,0,&ev);
	ev.data.fd=sfd;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	int retforw;
	int i,j;
	char buf[1024];
	n_cmd ncmd;
	int fdw=-1;
	int flag=-1;
	int len;
	while(1){
		retforw=epoll_wait(efd,evs,2,-1);
		for(i=0;i<retforw;i++){
			if(EPOLLIN==evs[i].events&&0==evs[i].data.fd){
				bzero(buf,sizeof(buf));
				read(0,buf,sizeof(buf));
				ret=com_cton(buf,&ncmd,&cs);
				if(-1==ret){
					continue;
				}
				send_c(sfd,&ncmd);
				printf("wait...\n");
				if(3==ncmd.key){
					bzero(buf,sizeof(buf));
					strcpy(buf,ncmd.argv);
					buf[strlen(buf)-1]=0;
					send_file(sfd,buf);
				}
			}
			if(EPOLLIN==evs[i].events&&sfd==evs[i].data.fd){
				recv_n(sfd,(char*)&len,sizeof(int));
				if(len>0){
					bzero(buf,sizeof(buf));
					recv_n(sfd,buf,len);
					printf("%s",buf);
				}else if(0==len&&4==ncmd.key){
					if(-1==fdw){
						bzero(buf,sizeof(buf));
						strcpy(buf,ncmd.argv);
						buf[strlen(buf)-1]=0;
						fdw=open(buf,O_CREAT|O_EXCL|O_WRONLY,0666);
					}else{
						close(fdw);
						fdw=-1;
					}
				}else{
					len=0-len;
					bzero(buf,sizeof(buf));
					recv_n(sfd,buf,len);
					write(fdw,buf,len);
				}
			}
		}
	}
}
