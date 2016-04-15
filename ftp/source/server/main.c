#include "factory.h"
#include "command.h"
int send_n(int fd,char *start,int len){
	int total=0;
	int one;
	while(total<len){
		one=send(fd,start+total,len-total,0);
		if(-1==one){
			printf("send error\n");
			return -1;
		}
		total=total+one;
	}
	return 0;
}
int recv_n(int fd,char *start,int len){
	int total=0;
	int one;
	while(total<len){
		one=recv(fd,start+total,len-total,0);
		if(-1==one){
			printf("recv error\n");
			return -1;
		}
		total=total+one;
	}
	return 0;
}
void getMode(char *buf,mode_t mode){
	int counter=0;
	char m[5]="xwr-";
	int i=0;
	buf[ML-1-i++]=0;
	for(;i<10;i++){
		if(mode%2==1){
		buf[ML-1-i]=m[(i-1)%3];
		}else{
			buf[ML-1-i]=m[3];
		}
		mode>>=1;
	}
	if(mode==64){
		buf[0]='d';
		buf[1]=' ';
	}else if(mode==32){
		buf[0]='-';
		buf[1]=' ';
	}else{
		buf[0]='e';
		buf[1]=' ';
	}
}
int ls_ll(char *path,int fd){
    DIR *dir;
	data dd;
	if((dir=opendir(path))==NULL){//
		bzero(dd.buf,sizeof(dd.buf));
		sprintf(dd.buf,"Error path\n");
		dd.len=strlen(dd.buf);
		send_n(fd,(char*)&dd,dd.len+4);
	}
	struct dirent *d;
	struct stat bufs;
	char p[128];
	char m[128];
	char t[128];
	char u[128];
	char g[128];
	while((d=readdir(dir))!=NULL){
		bzero(p,sizeof(p));
		sprintf(p,"%s/%s",path,d->d_name);
		stat(p,&bufs);
		bzero(m,sizeof(m));
		bzero(t,sizeof(t));
		bzero(u,sizeof(u));
		bzero(g,sizeof(g));
		getMode(m,bufs.st_mode);
		strncpy(t,ctime(&bufs.st_mtime)+4,15);
		strcpy(u,getpwuid(bufs.st_uid)->pw_name);
		strcpy(g,getgrgid(bufs.st_gid)->gr_name);
		bzero(dd.buf,sizeof(dd.buf));
		sprintf(dd.buf,"%-10s %-10s %-10s %-10d %-10d %-20s %-30s\n",m,g,u,(int)bufs.st_size,bufs.st_nlink,d->d_name,t);
		dd.len=strlen(dd.buf);
		send_n(fd,(char*)&dd,dd.len+4);
	}
}
int priority(uid_t uid,const char *path){
	struct passwd *pw;
	pw=getpwuid(uid);
	struct stat buf;
	stat(path,&buf);
	if(buf.st_uid==uid){
		return 0;//文件所有者
	}
	if(buf.st_gid==pw->pw_gid){
		return 1;//文件所有者同组
	}
	return 2;//游客访问
}
int sign_in(uid_t *uid,int fd){
	int len;
	data d;
	lable:
	bzero(d.buf,sizeof(d.buf));
	sprintf(d.buf,"Input your username:\n");
	d.len=strlen(d.buf);
	send_n(fd,(char*)&d,d.len+4);
	recv_n(fd,(char*)&len,sizeof(int));
	if(len<=0){
		bzero(d.buf,sizeof(d.buf));
		sprintf(d.buf,"Error format,you may try again\n");
		d.len=strlen(d.buf);
		send_n(fd,(char*)&d,d.len+4);
		goto lable;
	}
	char *buf=(char*)malloc(len+1);
	bzero(buf,sizeof(buf));
	recv_n(fd,buf,len);
	struct spwd *sp;
	sp=getspnam(buf);
	if(NULL==sp){
		bzero(d.buf,sizeof(d.buf));
		sprintf(d.buf,"Not exist,you may try again\n");
		d.len=strlen(d.buf);
		send_n(fd,(char*)&d,d.len+4);
		goto lable;
	}
	lable2:
	bzero(d.buf,sizeof(d.buf));
	sprintf(d.buf,"Input your password:\n");
	d.len=strlen(d.buf);
	send_n(fd,(char*)&d,d.len+4);
	recv_n(fd,(char*)&len,sizeof(int));
	if(len<=0){
		bzero(d.buf,sizeof(d.buf));
		sprintf(d.buf,"Error format,you may try again\n");
		d.len=strlen(d.buf);
		send_n(fd,(char*)&d,d.len+4);
		goto lable2;
	}
	char *buf1=(char*)malloc(len+1);
	bzero(buf1,sizeof(buf1));
	recv_n(fd,buf1,len);
	int i;
	int j;
	for(i=0,j=0;i<3;j++){
		if(sp->sp_pwdp[j]=='$'){
			i++;
		}
	}
	char salt[256]={0};
	strncpy(salt,sp->sp_pwdp,j-1);
	strcpy(salt,crypt(buf1,salt));
	if(strcmp(sp->sp_pwdp,salt)){
		free(buf);
		buf=NULL;
		free(buf1);
		buf1=NULL;
		return -1;
	}else{
		struct passwd *pwd;
		pwd=getpwnam(buf);
		*uid=pwd->pw_uid;
		free(buf);
		buf=NULL;
		free(buf1);
		buf1=NULL;
		return 0;
	}
}
int recv_c(int fd,n_cmd* command){
	int len;
	int ret;
	ret=recv_n(fd,(char*)&len,sizeof(int));
	if(-1==ret){
		return -1;
	}
	if(len<0){
		return -1;
	}
	ret=recv_n(fd,(char*)command,len);
	if(-1==ret){
		return -1;
	}
	return 0;
}
//发一个0表示切换开关状态，发负值作为文件的标识
int send_file(int fd,const char *path){
	int fdr;
	off_t off;
	data d;
	struct stat status;
	fdr=open(path,O_RDONLY);
	if(-1==fdr){
		bzero(d.buf,sizeof(d.buf));
		sprintf(d.buf,"文件不存在，看清楚再来\n");
		d.len=strlen(d.buf);
		send_n(fd,(char*)&d,4+strlen(d.buf));
		return -1;
	}
	int ret;
	int flag=0;
	send_n(fd,(char*)&flag,sizeof(int));
	recv_n(fd,(char*)&off,sizeof(off_t));
	stat(path,&status);
	off_t way=0;
	if(status.st_size-off>MS){
		//做映射
		way=status.st_size-off;
		d.len=0-sizeof(way);
		memcpy(d.buf,&way,sizeof(way));
		send_n(fd,(char*)&d,sizeof(way)+4);
		char *cur,*p;
		p=(char*)mmap(NULL,way,PROT_READ,MAP_PRIVATE,fdr,off);
		cur=p;
		int once,total;
		total=0;
		while(1){
			bzero(d.buf,sizeof(d.buf));
			if(way-total>sizeof(d.buf)){
				memcpy(d.buf,cur,sizeof(d.buf));
				d.len=0-sizeof(d.buf);
				send_n(fd,(char*)&d,0-d.len+4);
				total+=sizeof(d.buf);
				cur+=sizeof(d.buf);
				printf("total=%d\n",total);
			}else{
				printf("last time\n");
				memcpy(d.buf,cur,way-total);
				d.len=0-(way-total);
				send_n(fd,(char*)&d,0-d.len+4);
				break;
			}
		}
		munmap(p,way);
	}else{
		lseek(fdr,off,SEEK_SET);
		send_n(fd,(char*)&way,sizeof(int));
		while(1){
			bzero(d.buf,sizeof(d.buf));
			ret=read(fdr,d.buf,sizeof(d.buf)-1);
			if(-1==ret){
				bzero(d.buf,sizeof(d.buf));
				sprintf(d.buf,"服务器炸了，请重试\n");
				d.len=strlen(d.buf);
				send_n(fd,(char*)&d,4+strlen(d.buf));
				send_n(fd,(char*)&flag,sizeof(int));
				return -1;
			}else if(0==ret){
				break;
			}else{
				d.len=0-ret;
				send_n(fd,(char*)&d,ret+4);
			}
		}
	}
	send_n(fd,(char*)&flag,sizeof(int));
	bzero(d.buf,sizeof(d.buf));
	sprintf(d.buf,"发完了\n");
	d.len=strlen(d.buf);
	send_n(fd,(char*)&d,4+strlen(d.buf));
	close(fdr);
	return 0;
}
int recv_file(int fd,const char *path){
	int fdw;
	data d;
	printf("receive file into %s\n",path);
	fdw=open(path,O_CREAT|O_EXCL|O_WRONLY,0666);
	if(-1==fdw){
		bzero(d.buf,sizeof(d.buf));
		sprintf(d.buf,"文件已存在，看清楚再来\n");
		d.len=strlen(d.buf);
		send_n(fd,(char*)&d,4+strlen(d.buf));
		return -1;
	}
	int len=0;
	send_n(fd,(char*)&len,sizeof(int));
	char buf[1024];
	while(1){
		recv_n(fd,(char*)&len,sizeof(int));
		if(0==len){
			break;
		}
		bzero(buf,sizeof(buf));
		recv_n(fd,buf,len);
		write(fdw,buf,len);
	}
	close(fdw);
	len=0;
	send_n(fd,(char*)&len,sizeof(int));
	return 0;
}
int isvalid(int key){
	return 0<=key&&key<=256?1:0;
}
void* func(void* p){
	//接收文件描述符
	//就这里需要改，流程已经不再简单
	pfac f=(pfac)p;
	int fd;
	int ret;
	data d;
	uid_t uid;
	char abpath[512];
	char buf[512];
	n_cmd command;
	int i;
	DIR *dir;
	char *cur;
	int j;
	time_t t;
	struct stat status;
	char mode[20];
	while(1){
		//响应客户端请求，等待factory的任务分配
		fd=assign(f);
	//验证登陆，获取用户uid
	while(1){
		ret=sign_in(&uid,fd);
		if(-1==ret){
			bzero(d.buf,sizeof(d.buf));
			sprintf(d.buf,"sign_in failed,try again\n");
			d.len=strlen(d.buf);
			send_n(fd,(char*)&d,d.len+4);
			continue;
		}
		break;
	}
		send_n(fd,(char*)&ret,sizeof(int));
		strcpy(abpath,getcwd(NULL,0));
		while(1){
			//接收命令
			bzero(&command,sizeof(command));
			ret=recv_c(fd,&command);
			if(-1==ret||!isvalid(command.key)){
				bzero(d.buf,sizeof(d.buf));
				sprintf(d.buf,"command recieve error,try again\n");
				d.len=strlen(d.buf);
				send_n(fd,(char*)&d,4+strlen(d.buf));
			}else{
				bzero(d.buf,sizeof(d.buf));
				sprintf(d.buf,"wait...\n");
				d.len=strlen(d.buf);
				send_n(fd,(char*)&d,4+strlen(d.buf));
			}
			char** cargv=(char**)malloc(command.argc*sizeof(char*));//参数分离
			cur=command.argv;
			i=0;
			while(*cur!='\n'){
				if(i==0){
					cargv[i]=cur;
					i++;
				}else if(' '==*cur){
					*cur=0;
					cargv[i]=cur+1;
					i++;
				}
				cur++;
			}
			*cur=0;
			//解析命令并执行
			bzero(buf,sizeof(buf));
			time(&t);
			sprintf(buf,"Command key:%d,Excel time:%s",command.key,ctime(&t));
			syslog(LOG_INFO,"%s",buf);
			switch(command.key){
				case 0:
					//abpath
					if(!strcmp(".",cargv[0])){//等效于pwd
						bzero(d.buf,sizeof(d.buf));
						sprintf(d.buf,"already in %s\n",abpath);
						d.len=strlen(d.buf);
						send_n(fd,(char*)&d,d.len+4);
					}else if(cargv[0][0]=='.'&&cargv[0][1]=='.'){
						cur=cargv[0];
						i=0;
						while(*cur!=0){
							if(*cur=='.'){
								i++;
							}
							cur++;
						}
						if(*(cur=&abpath[strlen(abpath)-1])=='/'){
							*cur=0;
						}
						i=i/2;
						while(i>0&&cur!=abpath){
							if(*cur=='/'){
								i--;
							}
							cur--;
						}
						if(i>0){
							bzero(abpath,sizeof(abpath));
							strcpy(abpath,"/home");
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"Not exist,now you are in the home%s\n",abpath);
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
						}
						bzero(cur+1,sizeof(abpath)-strlen(abpath));
						bzero(d.buf,sizeof(d.buf));
						sprintf(d.buf,"now you are in %s\n",abpath);
						d.len=strlen(d.buf);
						send_n(fd,(char*)&d,d.len+4);
					}else if(!strcmp("~",cargv[0])){
						cur=abpath+1;
						i=0;
						while(*cur!='/'){
							cur++;
							i++;
						}
						bzero(cur,sizeof(abpath)-i);
						bzero(d.buf,sizeof(d.buf));
						sprintf(d.buf,"now you are in %s\n",abpath);
						d.len=strlen(d.buf);
						send_n(fd,(char*)&d,d.len+4);
					}else{
						bzero(buf,sizeof(buf));
						strcpy(buf,abpath);
						strcat(buf,"/");
						strcat(buf,cargv[0]);
						dir=opendir(buf);
						if(NULL==dir){
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"Not exist!\nNow you are in %s\n",abpath);
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
						}else{
							strcpy(abpath,buf);
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"now you are in %s\n",abpath);
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
						}
						closedir(dir);
					}
					break;
				case 1:
					bzero(buf,sizeof(buf));
					strcpy(buf,abpath);
					strcat(buf,"/");
					strcat(buf,cargv[0]);
					ls_ll(buf,fd);	
					break;
				case 2:
					bzero(d.buf,sizeof(d.buf));
					sprintf(d.buf,"Now you are in %s\n",abpath);
					d.len=strlen(d.buf);
					send_n(fd,(char*)&d,d.len+4);
					break;
				case 4:
					bzero(buf,sizeof(buf));
					strcpy(buf,abpath);
					strcat(buf,"/");
					strcat(buf,cargv[0]);
					send_file(fd,buf);
					break;
				case 3:
					bzero(buf,sizeof(buf));
					strcpy(buf,abpath);
					strcat(buf,"/");
					strcat(buf,cargv[0]);
					recv_file(fd,buf);
					break;
				case 5:
				//删除文件需要文件所在目录的写和执行权限
					bzero(buf,sizeof(buf));
					strcpy(buf,abpath);
					strcat(buf,"/");
					strcat(buf,cargv[0]);
					ret=priority(uid,abpath);
					bzero(mode,sizeof(mode));
					stat(abpath,&status);
					getMode(mode,status.st_mode);
					cur=mode+strlen(mode)-(2-ret)*3;
					if(*(cur-1)!='-'&&*(cur-2)!='-'){
						unlink(buf);
					}else{
						bzero(d.buf,sizeof(d.buf));
						sprintf(d.buf,"No enough priority\n");
						d.len=strlen(d.buf);
						send_n(fd,(char*)&d,d.len+4);
					}
					break;
				case 6:	
					bzero(buf,sizeof(buf));
					strcpy(buf,abpath);
					strcat(buf,"/");
					strcat(buf,cargv[0]);
					switch(priority(uid,buf)){
						case 0:
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"You are the owner\n");
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
							break;
						case 1:
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"You are in the same group with the owner\n");
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
							break;
						case 2:
							bzero(d.buf,sizeof(d.buf));
							sprintf(d.buf,"You are only a visitor\n");
							d.len=strlen(d.buf);
							send_n(fd,(char*)&d,d.len+4);
							break;
					}
					break;
				case 8:
					bzero(d.buf,sizeof(d.buf));
					sprintf(d.buf,"%s is rigou\n%s and %s is watching\n",cargv[0],cargv[1],cargv[2]);
					d.len=strlen(d.buf);
					send_n(fd,(char*)&d,d.len+4);
					break;
				case 9:
					bzero(d.buf,sizeof(d.buf));
					sprintf(d.buf,"%s,you are of great intelligence\n",cargv[0]);
					d.len=strlen(d.buf);
					send_n(fd,(char*)&d,d.len+4);
					break;
				case 10:
					bzero(d.buf,sizeof(d.buf));
					sprintf(d.buf,"%s,you are a SB\n",cargv[0]);
					d.len=strlen(d.buf);
					send_n(fd,(char*)&d,d.len+4);
					break;
				default:
					printf("key=%d\n",command.key);
			}
			free(cargv);
		}
	}
}
int main(int argc,char *argv[]){//ip port pnum capability
	if(5!=argc){
		printf("error args\n");
		return -1;
	}
	int capability=atoi(argv[4]);
	int pnum=atoi(argv[3]);
	factory f;
	fac_init(&f,pnum,capability,func);
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in ad;
	ad.sin_family=AF_INET;
	ad.sin_port=htons(atoi(argv[2]));
	ad.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ad,sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1;
	}
	ret=listen(sfd,capability);
	if(-1==ret){
		perror("listen");
		return -1;
	}
	int efd;
	efd=epoll_create(1);
	struct epoll_event ev;
	ev.events=EPOLLIN;
	ev.data.fd=sfd;
	epoll_ctl(efd,EPOLL_CTL_ADD,sfd,&ev);
	pnode n;
	struct sockaddr_in client;
	int len=sizeof(struct sockaddr);
	time_t t;
	char buf[1024];
	while(1){
		ret=epoll_wait(efd,&ev,1,-1);
		if(ret>0){
			if(ev.events==EPOLLIN&&ev.data.fd==sfd){
				n=(pnode)malloc(sizeof(node));
				n->fd=accept(sfd,(struct sockaddr*)&client,&len);
				if(n->fd<0){
					perror("accept");
					return -1;
				}
				time(&t);
				bzero(buf,sizeof(buf));
				sprintf(buf,"Client IP:%s,Client Port:%d,Link time:%s",inet_ntoa(client.sin_addr),ntohs(client.sin_port),ctime(&t));
				syslog(LOG_INFO,"%s",buf);
				n->next=NULL;
				visit(&f,n);
			}	
		}
	}
}
