#include "fun.h"

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("error args\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//端口转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	ret=listen(sfd,LNUM);
	if(-1==ret)
	{
		perror("listen");
		return -1;
	}
	struct sockaddr_in client;
	int len=sizeof(struct sockaddr);
	int new_fd=-1;
	fd_set rdset;
	fd_set newset;
	int maxfd;
	FD_ZERO(&newset);
	char buf[128]={0};
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(0,&newset);
		FD_SET(sfd,&newset);
		memcpy(&rdset,&newset,sizeof(rdset));
		if(FD_ISSET(new_fd,&newset))
		{
			maxfd=new_fd+1;
		}else{
			maxfd=sfd+1;
		}
		ret=select(maxfd,&rdset,NULL,NULL,NULL);
		if(ret >0)
		{
			if(FD_ISSET(sfd,&rdset))
			{	
				new_fd=accept(sfd,(struct sockaddr*)&client,&len);	
				if(-1==new_fd)
				{
					perror("accept");
					return -1;
				}
				printf("client ip=%s,port=%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
				FD_SET(new_fd,&newset);
			}
			if(FD_ISSET(new_fd,&rdset))
			{
				bzero(buf,sizeof(buf));
				ret=recv(new_fd,buf,sizeof(buf),0);
				if(ret<0)
				{
					perror("recv");
					return -1;
				}else if(ret ==0)
				{
					close(new_fd);
					FD_CLR(new_fd,&newset);
				}else{
					printf("%s\n",buf);
				}
			}
			if(FD_ISSET(0,&rdset))
			{
				bzero(buf,sizeof(buf));
				read(0,buf,sizeof(buf));
				ret=send(new_fd,buf,strlen(buf)-1,0);
				if(ret<0)
				{
					perror("send");
					return -1;
				}
			}
		}
	}
	close(sfd);
}
