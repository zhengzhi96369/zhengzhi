#ifndef __FUN_H__
#define __FUN_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <shadow.h>
#include <pwd.h>
#include <dirent.h>
#include <crypt.h>
#include <grp.h>
#include <time.h>
#define F "file"
typedef struct{
	int len;
	char buf[1000];
}data;
#endif
