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
	return 0;
}
int main(int argc,char *argv[]){
	if(2!=argc){
		printf("error args\n");
		return -1;
	}
	int sfds[2];
	int ret;
	ret=socketpair(AF_LOCAL,SOCK_STREAM,0,sfds);
	if(-1==ret){
		perror("socketpair");
		return -1;
	}
	if(!fork()){
		close(sfds[1]);
		int fd;
		ret=recvfd(sfds[0],&fd);
		if(-1==ret){
			printf("receive failed!\n");
			return -1;
		}
		printf("The fd is %d\n",fd);
		char buf1[10]={0};
		char buf2[10]={0};
		struct iovec vecs[2];
		vecs[0].iov_base=buf1;
		vecs[1].iov_base=buf2;
		vecs[0].iov_len=7;
		vecs[1].iov_len=7;
		lseek(fd,0,SEEK_SET);
		ret=readv(fd,vecs,2);
		if(-1==ret){
			perror("readv");
			return -1;
		}
		printf("buf1:%-10s\nbuf2:%-10s\n",buf1,buf2);
		exit(0);
	}else{
		close(sfds[0]);
		int fd;
		fd=open(argv[1],O_RDWR);
		if(-1==fd){
			perror("open");
			return -1;
		}
		printf("The fd is %d\n",fd);
		char buf1[10]="wo shi ";
		char buf2[10]="ni baba";
		struct iovec vecs[2];
		vecs[0].iov_base=buf1;
		vecs[1].iov_base=buf2;
		vecs[0].iov_len=strlen(buf1);
		vecs[1].iov_len=strlen(buf2);
		ret=writev(fd,vecs,2);
		if(-1==ret){
			perror("writev");
			return -1;
		}
		ret=sendfd(sfds[1],fd);
		if(-1==ret){
			printf("send failed!\n");
			return -1;
		}
		wait(NULL);
		return 0;
	}
}
