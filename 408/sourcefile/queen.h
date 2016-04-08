#ifndef __QUEEN_H__
#define __QUEEN_H__
#include "fun.h"
typedef struct node{
	int fd;
	struct node *next;
}node,*pnode;
typedef struct{
	pnode head,tail;
	int size;
}queen,*pq;
void q_init(pq);
void q_add(pq,pnode);
int q_get(pq,pnode*);//0表示没有
int q_getlen(const pq);
#endif
