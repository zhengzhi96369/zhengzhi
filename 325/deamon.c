//task:To creat a deamon process
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int main(){
	int i;
	if((i=fork())==0){
		setsid();
		chdir("/");
		umask(0);
		int i;
		for(i=0;i<3;i++){
			close(i);
		}
		while(1){
			sleep(10);
		}
	}else{
		printf("His pid is %d\n",i);
		exit(0);
	}
}
