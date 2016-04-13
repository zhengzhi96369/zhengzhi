#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
void h(int sig){
	printf("I'm %dsig\n",sig);
	sleep(5);
	printf("%dsig close\n",sig);
}
void main(){
	signal(SIGINT,h);
	signal(SIGQUIT,h);
	while(1);
}
