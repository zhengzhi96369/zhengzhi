#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
char *pc;
void* tfunc(void* p){
	printf("Here is the new pthread%d!\n",(int)p);
//	pthread_exit((void*)10);
//	pc=(char*)malloc(10);
//	memset(pc,0,10);
//	pc[0]='z';
//	puts(pc);
	printf("woca");
	sleep(5);
//	while(1){
//		printf("keep\n");
//	}
	return (void*)0;
}
int main(){
	pthread_t thr1;
	pthread_attr_t ta1;
	//pthread_creat(phtread_t *pthread,pthread_attr_t *ta,void* (*start)(void*),void *args);
	int ret;
	ret=pthread_create(&thr1,NULL,tfunc,(void*)1);
	if(0!=ret){
		printf("Failed create!\n");
		return -1;
	}
	sleep(3);
	pthread_cancel(thr1);//这个会把缓冲区刷出来，类似于exit
	printf("%d\n",pthread_join(thr1,(void**)&ret));//这是一个阻塞函数，回收只是把相应的存在需要的东西清空了
	printf("ret=%d\n",ret);//线程函数正常退出返回的是0,错误返回-1
//	puts(pc);
	return 0;
}
