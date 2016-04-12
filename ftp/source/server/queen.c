#include "queen.h"
void q_init(pq q){
	q->head=NULL;
	q->tail=NULL;
	q->size=0;
}
void q_add(pq q,pnode n){
	if(0==q->size){
		q->head=n;
		q->tail=n;
	}else{
		q->tail->next=n;
		q->tail=n;
	}
	q->size++;
}
int q_get(pq q,pnode *n){
	if(0==q->size){
		return 0;
	}else if(1==q->size){
		*n=q->head;
		q->head=NULL;
		q->tail=NULL;
	}else{
		*n=q->head;
		q->head=q->head->next;
	}
	q->size--;
	return 1;
}
int q_getlen(const pq q){
	return q->size;
}
