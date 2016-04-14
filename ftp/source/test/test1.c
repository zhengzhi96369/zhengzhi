#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
void main(){
	int fd1,fd2,fd3;
	fd1=open("testfile2",O_CREAT|O_RDWR,0666);
	fd2=open("testfile2",O_RDWR);
	fd3=open("testfile2",O_RDWR);
	printf("%d %d %d",fd1,fd2,fd3);
}
