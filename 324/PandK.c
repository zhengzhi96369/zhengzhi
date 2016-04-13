#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
int isBD(char c){
	return c==','||c==';'||c=='\''||c=='!'||c=='.'?1:0;
}
int main(int argc,char *argv[]){
	if(2!=argc){
		printf("error args!\n");
		return -1;
	}
	int fd;
	if((fd=open(argv[1],O_RDWR))==-1){
		perror("open file");
		return -1;
	}
	char *cur,*s;
	if((s=(char*)mmap(NULL,1<<10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0))==NULL){
		perror("mmap");
		return -1;
	}
	cur=s;
	int kpid;
	kpid=fork();
	if(!kpid){
	//在子进程中fork返回了0
		printf("Hello,here comes the kid!\nmy id:%d\nmy parent:%d\n",getpid(),getppid());
		while(*cur){
			if(*cur>='a'&&*cur<='z'){
				*cur-=32;
			}
			cur++;
		}
		exit(10);
	}else{
	//在父进程中返回了子进程的id
		int stat;
		wait(&stat);
		printf("%d\n",stat);
		if(WIFEXITED(stat)){
			printf("My kid end with %d\n",WEXITSTATUS(stat));
		}
		printf("Hello,here comes the process created by your programme!\nmy id:%d\nmy kid:%d\n",getpid(),kpid);
		while(*cur){
			if(isBD(*cur)){
				*cur=32;
			}
			cur++;
		}
		munmap(s,1<<10);
		return 0;
	}
}
