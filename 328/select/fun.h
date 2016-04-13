#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/select.h>
#include <sys/ipc.h>
typedef struct c{
	int lock;
	int read;
	char buf[1024];
}chat,*pchat;
void lock(pchat p){
	while(p->lock==1);
	p->lock=1;
}
void unlock(pchat p){
	p->lock=0;
}
