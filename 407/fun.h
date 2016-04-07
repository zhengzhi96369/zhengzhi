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
#define DOWNLOAD "file"
typedef struct c{
	int pid;
	int fd;
	int busy;
}child,*pchild;
typedef struct d{
	int len;
	char buf[1000];
}data;
void nonblock(int*);
void make_child(pchild,int);
void child_handle(int);
int send_file(int);
int recv_n(int,char*,int);
int send_n(int,char*,int);
