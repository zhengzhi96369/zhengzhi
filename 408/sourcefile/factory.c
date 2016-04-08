#include "factory.h"
void fac_init(pfac f,int pnum,int capability,pthfunc func){
	f->pths=(pthread_t*)malloc(pnum*sizeof(pthread_t));
	int i;
	int ret;
	for(i=0;i<pnum;i++){
		ret=pthread_create(&f->pths[i],NULL,func,(void*)f);
		if(ret!=0){
			perror("fac_init:create");
			exit(-1);
		}
	}
	f->pnum=pnum;
	f->func=func;
	q_init(&f->qwait);
	f->capability=capability;
	pthread_mutex_init(&f->mutex,NULL);
	pthread_cond_init(&f->cond,NULL);
	f->flag=0;
}
int assign(pfac f){
	int ret;
	pnode p;
	pthread_mutex_lock(&f->mutex);
	while(!q_get(&f->qwait,&p)){
		pthread_cond_wait(&f->cond,&f->mutex);
	}
	ret=p->fd;
	free(p);
	pthread_mutex_unlock(&f->mutex);
	return ret;
}
void visit(pfac f,pnode n){
	int size;
	pthread_mutex_lock(&f->mutex);
	size=q_getlen(&f->qwait);
	if(size==f->capability){
		//超载，不予响应
	}else{
		q_add(&f->qwait,n);
	}
	pthread_mutex_unlock(&f->mutex);
	pthread_cond_signal(&f->cond);
}
