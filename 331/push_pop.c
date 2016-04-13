#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void cleaner(void *p){
	char **s=p;
	printf("cleaner\n");
	puts(*s);
	free(*s);
	*s=NULL;
}
void *tfunc(void *p){
	printf("I'm here\n");
	char *s=(char*)malloc(10);
	memcpy(p,&s,sizeof(p));
	pthread_cleanup_push(cleaner,p);
	strcpy(s,"hehe");
	pthread_cleanup_pop(0);
}
int main(){
	pthread_t th1,th2;
	int ret;
    char* p;
	ret=pthread_create(&th1,NULL,tfunc,(void*)&p);
	if(-1==ret){
		printf("creat failed!\n");
		return -1;
	}
/*	pthread_create(&th2,NULL,tfunc,NULL);
	if(-1==ret){
		printf("creat failed!\n");
		return -1;
	}*/
	pthread_join(th1,NULL);
//	pthread_join(th2,NULL);
	printf("%d\n",(int)p);
	puts(p);
}
