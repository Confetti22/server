 ///
 /// @file    tcp.h
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2022-05-28 08:24:11
 ///
 
#ifndef __WD_TCPHEADER_H__
#define __WD_TCPHEADER_H__
#include<func.h>
#define BUFSIZE 4096
#define SENDBUFSIZE 4096
typedef struct {
	int length;
	char data[BUFSIZE];
} train_t;

//抽象出一个通道层
typedef struct Channel {
	int  sockfd;
	int  recvbufSize;		//接收缓冲区已经存放了多少数据
	char recvbuff[BUFSIZE];//应用层接收缓冲区
	int  sendbufSize;		//发送缓冲区还有多少数据
	char sendbuff[BUFSIZE];//应用层发送缓冲区
} channel_t;

void setNonblock(int fd);

int socketInit();

struct sockaddr_in inetaddress(const char * ip, uint16_t port);

int tcpServerInit(const char * ip, uint16_t port);

int sendn(int fd, const void * buf, int len);


void channelsInit(channel_t * p, int length);

//针对于某一个通道的销毁
void channelInit(channel_t * p);
void channelDestroy(channel_t * p);
void clearRecvBuff(channel_t * p);
void clearSendBuff(channel_t * p);
//通道数组中增删查
void channelAdd(channel_t * p, int length, int fd);
void channelDel(channel_t * p, int length, int fd);
//>=0 查找到了某一个fd
//-1  表示没有找到
int channelGetIndex(channel_t* p, int length, int fd);
 

//epoll相关操作进行封装

int epollCreate();
void EpollAdd(int epfd,int fd,uint32_t EVENTS);
void EpollMod(int epfd,int fd,uint32_t EVENTS);
void EpollDel(int epfd,int fd);


void epoll_add_rw_event(int epfd, int fd);
void epoll_add_r_event(int epfd, int fd);
void epoll_add_w_event(int epfd, int fd);
void epoll_mod_rw_event(int epfd, int fd);
void epoll_mod_r_event(int epfd, int fd);
void epoll_mod_w_event(int epfd, int fd);
void epoll_del_event(int epfd, int fd);

//向peerfd 发送 filename文件
void sendFile(int peerfd, const char * filename);

//从peerfd 接受 filename文件
void recvFile(int peerfd, const char * filename);



//void epollHandleReadEvent(int epfd, int fd);
//void epollHandleWriteEvent(int epfd, int fd);
 
#endif
