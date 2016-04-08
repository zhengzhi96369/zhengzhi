#ifndef __FACTORY_H__
#define __FACTORY_H_
#include "fun.h"
#include "queen.h"
typedef void* (*pthfunc)(void*);
typedef struct{
	pthread_t *pths;//线程池
	int pnum;
	pthfunc func;
	queen qwait;//等待处理的队列
	int capability;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int flag;//标志位
}factory,*pfac;
void fac_init(pfac,int,int,pthfunc);
int assign(pfac);
void visit(pfac,pnode);
#endif
