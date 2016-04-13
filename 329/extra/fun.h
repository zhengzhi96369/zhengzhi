#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <signal.h>
#define L 1024
typedef struct chat{
	int isnew;
	char buf[L];
}*pchat,chat;
void handle(int sig){
	kill(0,SIGKILL);
}
