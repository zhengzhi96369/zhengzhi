#include "fun.h"
int sendfd(int fdw,int fd){
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct iovec vec;
	vec.iov_base="NULL";
	vec.iov_len=4;
	msg.msg_iov=&vec;
	msg.msg_iovlen=1;
	//变长结构体
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr*)malloc(len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_type=SOL_SOCKET;
	cmsg->cmsg_level=SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;//获得数据区的首地址
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(fdw,&msg,0);
	if(-1==ret){
		perror("sendmsg");
		return -1;
	}
	return 0;
}
int recvfd(int fdr,int *fd){
	struct msghdr msg;
	bzero(&msg,sizeof(msg));
	struct iovec vec;
	char buf[10]={0};
	vec.iov_base=buf;
	vec.iov_len=10;
	msg.msg_iov=&vec;
	msg.msg_iovlen=1;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr*)malloc(len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_type=SOL_SOCKET;
	cmsg->cmsg_level=SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(fdr,&msg,0);
	if(-1==ret){
		perror("recvmsg");
		return -1;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
}
void nonblock(int *fd){
	int stat;
	stat=fcntl(*fd,F_GETFL);
	stat=stat|O_NONBLOCK;
	stat=fcntl(*fd,F_SETFL,stat);
}
void makechild(pchild p,int num){
	int sfds[2];
	pid_t pid;
	int i;
	for(i=0;i<num;i++){
		socketpair(AF_LOCAL,SOCK_STREAM,0,sfds);
		if(!(pid=fork())){
			close(sfds[0]);
			child_handle(sfds[1]);
		}else{
			close(sfds[1]);
			p[i].pid=pid;
			p[i].fd=sfds[0];
			p[i].busy=0;
		}
	}
}
void child_handle(int fd){
	while(1){
		//接收文件描述符
		int fds;
		int ret;
		ret=recvfd(fd,&fds);
		if(-1==ret){
			printf("recvfd failed\n");
			exit(-1);
		}
		//发送文件
		ret=send_file(fds);
		if(-1==ret){
			printf("send file failed\n");
			exit(-1);
		}
		//通知结束
		int flag=1;
		ret=write(fd,&flag,sizeof(int));
		if(-1==ret){
			perror("write");
			exit(-1);
		}
	}
}
int send_file(int fd){
	int fdr=open(DOWNLOAD,O_RDONLY);
	if(-1==fdr){
		perror("open");
		return -1;
	}
	printf("Ready to pass the file\n");
	data d;
	d.len=strlen(DOWNLOAD);
	strcpy(d.buf,DOWNLOAD);
	int ret;
	ret=send_n(fd,(char*)&d,4+d.len);
	if(-1==ret){
		printf("send file name failed");
		return -1;
	}
	printf("filename passed\n");
	while(1){
		bzero(d.buf,sizeof(d.buf));
		ret=read(fdr,d.buf,sizeof(d.buf));
		if(-1==ret){
			perror("read");
			return -1;
		}else if(0==ret){
			break;
		}else{
			d.len=ret;
			ret=send_n(fd,(char*)&d,4+d.len);
			if(-1==ret){
				printf("send file failed\n");
				return -1;
			}
		}

	}
	int flag=0;
	ret=send_n(fd,(char*)&flag,4);
	if(-1==ret){
		printf("unexpected end of send\n");
		return -1;
	}
}
int send_n(int fds,char *start,int len){
	int once;
	int total=0;
	while(total<len){
		once=send(fds,start+total,len-total,0);
		if(-1==once){
			perror("send");
			return -1;
		}
		total=total+once;
	}
	return 0;
}
int recv_n(int fd,char *start,int len){
	int once;
	int total=0;
	while(total<len){
		once=recv(fd,start+total,len-total,0);
		if(-1==once){
			perror("recv");
			return -1;
		}
		total=total+once;
	}
	return 0;
}
