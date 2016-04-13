#include <unistd.h>
#include <stdio.h>
int main(int argc,char *argv[]){
	if(3!=argc){
		printf("error args!\n");
		return -1;
	}
	//kill前一个是目标，后一个是sig
	kill(atoi(argv[1]),atoi(argv[2]));
	return 0;
}
