#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
void h(int sig){

}
void handle(int sig,siginfo_t *info,void *resevered){
	printf("%d is here\n",sig);
	printf("It's from %d,the user is %d\n",info->si_pid,info->si_uid);
	signal(SIGALRM,h);
	alarm(20);
	pause();
	printf("I am awake\n");
	sigset_t s;
	sigpending(&s);
	if(sigismember(&s,SIGQUIT)){
		printf("Here is a pending sigquit\n");
	}
}
int main(int argc,char *argv[]){
	struct sigaction ac;
	memset(&ac,0,sizeof(struct sigaction));
	ac.sa_sigaction=handle;
	ac.sa_flags=SA_SIGINFO;
	//处理sa_mask
	sigemptyset(&ac.sa_mask);
	sigaddset(&ac.sa_mask,SIGQUIT);
	sigaction(SIGINT,&ac,NULL);
	while(1);
}
