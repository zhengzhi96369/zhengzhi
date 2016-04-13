#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
int isBD(char c){
	return c=='['||c==']'||c==','?1:0;
}
int isUpper(char c){
	return 'A'<=c&&c<='Z'?1:0;
}
int main(int argc,char *argv[]){
	if(2!=argc){
		printf("error args!\n");
		return -1;
	}
	int fd;
	if((fd=open(argv[1],O_RDWR))==-1){
		perror("open");
	}
	char *c;
	c=(char*)mmap(NULL,1<<23,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	while(*c){
		if(isBD(*c)){
			*c=' ';
		}else if(isUpper(*c)){
			*c+=32;
		}
		c++;
	}
	return 0;
}
