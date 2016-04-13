#define BL 128
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc,char *argv[]){
	if(argc!=3){
		printf("error args!\n");
		return -1;
	}
	int fdforw,fdforr;
	if((fdforw=open(argv[1],O_WRONLY))==-1){
		perror("open");
		return -1;
	}
	if((fdforr=open(argv[2],O_RDONLY))==-1){
		perror("open");
		return -1;
	}
	printf("ready!\n");
	char buf[BL];
	fd_set set;
	int ret;
	while(1){
		FD_SET(0,&set);
		FD_SET(fdforr,&set);
		ret=select(fdforr+1,&set,NULL,NULL,NULL);
		if(ret>0){
			if(FD_ISSET(0,&set)){
				memset(buf,0,BL);
				read(0,buf,BL);
				write(fdforw,buf,strlen(buf)-1);
			}else{
				memset(buf,0,BL);
				read(fdforr,buf,BL);
				puts(buf);
			}
		}
	}
}
