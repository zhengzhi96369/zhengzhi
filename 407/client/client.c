#include "fun.h"
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);//这个描述符不简单，随意修改其状态可能导致connect失败
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	int ret;
	struct sockaddr_in ser;
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(-1==ret){
		perror("connect");
		return -1;
	}
	int fdw;
	char bufforname[128]={0};
	int len;
	printf("the file len = %d\n",len);
	ret=recv_n(sfd,(char*)&len,sizeof(int));
	if(-1==ret){
		printf("Cannot get the filename\n");
		return -1;
	}
	printf("the file len = %d\n",len);
	if(len<=0){
		printf("error filename\n");
		return -1;
	}
	ret=recv_n(sfd,bufforname,len);
	if(-1==ret){
		printf("filename receive failed\n");
		return -1;
	}
	puts(bufforname);
	fdw=open(bufforname,O_CREAT|O_RDWR,0666);
	if(-1==fdw){
		perror("file create");
		return -1;
	}
	char buf[1024];
	while(1){
		ret=recv_n(sfd,(char*)&len,sizeof(int));
		if(-1==ret){
			printf("data recieve interrupted\n");
			return -1;
		}
		if(0==len){
			break;
		}
		ret=recv_n(sfd,buf,len);
		if(-1==ret){
			printf("data recieve interrupted\n");
			return -1;
		}
		ret=write(fdw,buf,len);
		if(-1==ret){
			printf("file write failed\n");
			return -1;
		}
	}
	close(sfd);
	close(fdw);
	return 0;
}
