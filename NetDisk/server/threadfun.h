#ifndef __threadFunc_H__
#define __threadFunc_H__

#include"threadpool.h"
//主线程执行的功能
void Handle_events(int epfd,struct epoll_event* events,int nready,int listenfd);
void Handle_accept(int epfd,int listenfd);
int  Handle_exit(int epfd,int exitpipe);

//join线程池的所有子线程，并回收线程池申请的系统资源
 void Handle_command(int epfd,int peerfd);

//以下为子线程执行的功能
void* handle_register(void * arg);
void* handle_login(void * arg);
void* handle_puts(void * arg);
void* handle_gets(void * arg);
void* handle_cd(void * arg);
void* handle_rm(void * arg);
void* handle_ls(void * arg);
void* handle_mkdir(void * arg);
void* handle_pwd(void * arg);
//断开与对端的tcp链接
void* handle_exit(void * arg);

//根据pathname 改变当前目录*pcwd
bool cd(char* pathname,const int user_id, int * pcwd);

//user_id-->cwd
int Getcwd(const int user_id,int *pcwd);

//peerfd--->user_id, str_user_id
int  get_str_user_id(int peerfd,char *str_user_id);
#endif
