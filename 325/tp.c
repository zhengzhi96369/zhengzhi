#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){
	int fds[2];
	pipe(fds);
	if(!fork()){
		close(fds[0]);
		write(fds[1],"woca",4);
		close(fds[1]);
		exit(1);
	}else{
		close(fds[1]);
		char buf[10]={0};
		read(fds[0],buf,sizeof(buf));
		puts(buf);
		close(fds[0]);
	}
}
