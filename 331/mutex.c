#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
int sum=0;
pthread_mutex_t mutex;
void cleaner(void* p){
	printf("I'm the cleaner\n");
	int ret;
	ret=pthread_mutex_trylock(&mutex);//如果已经锁了就会给一个EBUSY
	printf("trylock returns %d\n",ret);
	pthread_mutex_unlock(&mutex);
}
void* tfunc(void *p){
	printf("create success\n");
	pthread_cleanup_push(cleaner,NULL);
	int i;
	for(i=0;i<5;i++){
		pthread_mutex_lock(&mutex);
		sum+=1;
		sleep(1);
		pthread_mutex_unlock(&mutex);
	}
	pthread_cleanup_pop(1);
}
int main(){
	pthread_mutexattr_t ma;
//	ma.__mutexkind=PTHREAD_MUTEX_ERRORCHECK_NP;
	int i=PTHREAD_MUTEX_ERRORCHECK_NP;
	memcpy(&ma,&i,sizeof(ma));
	pthread_mutex_init(&mutex,&ma);
	pthread_t th1,th2;
	pthread_create(&th1,NULL,tfunc,NULL);
	pthread_create(&th2,NULL,tfunc,NULL);
	sleep(2);
	pthread_cancel(th1);
	sleep(1);
	pthread_cancel(th2);
	pthread_join(th1,NULL);
	pthread_join(th2,NULL);
	printf("sum=%d\n",sum);
	return 0;
}
