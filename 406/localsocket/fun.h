#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BL 128
void nonblock(int *fd){
	int stat;
	stat=fcntl(*fd,F_GETFL);
	stat=stat|O_NONBLOCK;
	stat=fcntl(*fd,F_SETFL,stat);
}
