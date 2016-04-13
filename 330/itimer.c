#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
void handle(int sig){
	time_t t;
	time(&t);
	printf("%s\n",ctime(&t));
}
int main(){
	signal(SIGALRM,handle);
	kill(0,SIGALRM);
	struct itimerval rt;
	memset(&rt,0,sizeof(rt));
	rt.it_value.tv_sec=5;
	rt.it_interval.tv_sec=2;
	int ret;
	ret=setitimer(ITIMER_REAL,&rt,NULL);
	sleep(10);
	while(1);
	return 0;
}
