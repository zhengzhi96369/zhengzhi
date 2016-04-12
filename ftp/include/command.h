#ifndef __COMMAND_H__
#define __COMMAND_H__
#include "fun.h"
#define CL 16
#define ML 12
#define AL 256
//cd  进入对应目录
//ls 列出相应目录文件
//puts 将本地文件上传至服务器
//gets 文件名 下载服务器文件到本地
//remove 删除服务器上文件
//pwd 显示目前所在路径
//其他命令不响应
//cp
//mv
typedef struct net_cmd{
	int key;
	int mode;
	int argc;
	char argv[AL];
}n_cmd;
typedef struct cnode{
	char command[CL];
	int key;
	int argc;
	char mode[ML];
	struct cnode *next;
}c_node,*pc_node;
typedef struct cmdset{
	pc_node head;
}cset,*pcset;
void com_add(pc_node,pcset);//用不到
int com_ismember(char*,pc_node*,pcset);//用不到
int com_init(char* s,pcset);//初始化命令集合
int com_cton(const char*,n_cmd*,pcset);//命令串转化为网络命令，成功返回0
int com_getlen(n_cmd*);//获得实际应传的长度
#endif
