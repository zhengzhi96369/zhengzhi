#include "command.h"
void com_add(pc_node p,pcset set){
	p->next=set->head;
	set->head=p;
}
int com_init(char* s,pcset set)
{
	FILE *fp=fopen(s,"rb+");
	if(NULL==fp){
		perror("config open failed\n");
		return -1;
	}
	char buf[128];
	char* ret;
	pc_node p;
	while(1){
		ret=fgets(buf,sizeof(buf),fp);
		if(NULL==ret){
			break;
		}
		p=(pc_node)malloc(sizeof(c_node));
		sscanf(buf,"%s%d%d%s",p->command,&p->key,&p->argc,p->mode);
		p->next=NULL;
		com_add(p,set);
	}
	fclose(fp);
	return 0;
}
int com_ismember(char* s,pc_node *p,pcset set){
	pc_node c=set->head;
	while(c!=NULL){
		if(!strcmp(s,c->command))break;
		c=c->next;
	}
	if(c==NULL){
		return -1;
	}else{
		*p=c;
		return 0;
	}
}
int com_cton(const char* cmd,n_cmd* ncmd,pcset set)
{
	int i=0;
	while(cmd[i]!=' '&&cmd[i]!='\n'){
		i++;
	}
	if(i>CL){
		printf("error command\n");
		return -1;
	}
	char c[CL];
	bzero(c,sizeof(c));
	strncpy(c,cmd,i);
	pc_node p;
	int ret;
	ret=com_ismember(c,&p,set);
	if(-1==ret){
		printf("command not exists\n");
		return -1;
	}
	int k=++i;
	int j=0;
	if(cmd[i]!=0)
	{
		j=1;
		while(cmd[i]!=0){
			if(32==cmd[i]){
				j++;
			}
			i++;
		}
	}
	if(j!=p->argc){
		printf("error command format\n");
		return -1;
	}
	ncmd->key=p->key;
	ncmd->argc=p->argc;
	ncmd->mode=0;
	//mode 暂不实现
	bzero(ncmd->argv,AL);
	strcpy(ncmd->argv,cmd+k);
}
int com_getlen(n_cmd* ncmd){
	return 3*sizeof(int)+strlen(ncmd->argv);
}
