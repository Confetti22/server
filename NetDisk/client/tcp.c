 ///
 /// @file    tcp.c
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2022-05-28 08:26:36
 ///
 

#include "tcp.h"

void setNonblock(int fd)
{ 
	int flags = fcntl(fd, F_GETFL, 0); 
	flags |= O_NONBLOCK;
	fcntl(fd, F_SETFL, flags);
}

struct sockaddr_in inetaddress(const char * ip, uint16_t port)
{
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	return serveraddr;
}

int socketInit()
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0); 
	ERROR_CHECK(listenfd, -1, "socket");
	return listenfd;
}

int tcpServerInit(const char * ip, uint16_t port)
{
	int ret;
	//1. 创建tcp套接字
	int listenfd = socketInit();

	int reuse = 1;
	if(setsockopt(listenfd, SOL_SOCKET, 
			SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) 
	{	perror("setsockopt");	}

	//2. 绑定网络地址
	struct sockaddr_in serveraddr = inetaddress(ip, port);
	ret = bind(listenfd, 
			(struct sockaddr*)&serveraddr, 
			sizeof(serveraddr));
	ERROR_CHECK(ret, -1, "bind");

	//3. 进行监听
	ret = listen(listenfd, 100);
	ERROR_CHECK(ret, -1, "listen");
	printf("listen has completed.\n");
	return listenfd;
}


void channelsInit(channel_t * p, int length)
{
	for(int i = 0; i < length; ++i) {
		channelInit(p + i);
	}
}

void channelInit(channel_t * p) {
	if(p) {
		p->sockfd = -1;//未使用
		p->recvbufSize = 0;
		p->sendbufSize = 0;
		memset(p->recvbuff, 0, BUFSIZE);
		memset(p->sendbuff, 0, BUFSIZE);
	}
}

void channelDestroy(channel_t * p)
{
	if(p) {
		close(p->sockfd);
		channelInit(p);
	}
}

void channelAdd(channel_t * p, int length, int fd)
{
	for(int i = 0; i < length; ++i) {
		if(p[i].sockfd == -1) {
			p[i].sockfd = fd;//保存下来
			clearRecvBuff(p + i);
			clearSendBuff(p + i);
			break;
		}
	}
}

void channelDel(channel_t * p, int length, int fd)
{
	int idx = channelGetIndex(p, length, fd);
	if(idx != -1) {
		channelDestroy(p + idx);
	}
}

//>=0 查找到了某一个fd
//-1  表示没有找到
int channelGetIndex(channel_t* p, int length, int fd)
{
	if(p) {
		for(int i = 0; i < length; ++i) {
			if(p[i].sockfd == fd) {
				return i;//查找到了
			}
		}
		return -1;//没有找到
	}
}

void clearRecvBuff(channel_t * p)
{
	if(p) {
		p->recvbufSize = 0;
		memset(p->recvbuff, 0, BUFSIZE);
	}
}

void clearSendBuff(channel_t * p)
{
	if(p) {
		p->sendbufSize = 0;
		memset(p->sendbuff, 0, BUFSIZE);
	}
}

//epoll相关操作进行封装

int EpollCreate()
{
	int fd = epoll_create1(0);
	ERROR_CHECK(fd, -1, "epoll_create1");
	return fd;
}


void EpollAdd(int epfd, int fd, uint32_t EVENTS){
    struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EVENTS;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

void EpollMod(int epfd, int fd, uint32_t EVENTS){
    struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EVENTS;
	if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

void EpollDel(int epfd, int fd){
    struct epoll_event evt;
	evt.data.fd = fd;
	if(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}



void epoll_add_rw_event(int epfd, int fd){
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLIN | EPOLLOUT;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}


void epoll_add_r_event(int epfd, int fd){
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLIN;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

void epoll_add_w_event(int epfd, int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLOUT;
	if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

//初始情况下，监听的是读写事件
//现在希望他只监听读事件
void epoll_mod_w_event(int epfd, int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLOUT;
	if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

void epoll_mod_r_event(int epfd, int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLIN;
	if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

//初始情况下，监听的是读事件
//现在希望他监听读写事件
void epoll_mode_rw_event(int epfd, int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	evt.events = EPOLLIN | EPOLLOUT;
	if(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

void epoll_del_event(int epfd, int fd)
{
	struct epoll_event evt;
	evt.data.fd = fd;
	if(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt) < 0) {
		perror("epoll_ctl");
	}
}

int sendn(int fd, const void * buf, int len)
{
	int left = len;
	char * pbuf = (char *)buf;
	int ret;
	while(left > 0) {
		ret = send(fd, pbuf, left, 0);
		if(ret < 0) {
			perror("send");
			break;
		} else {
			pbuf += ret;
			left -= ret;
		}
	}
	return len - left;
}
//发送本地的filename给peerfd
void sendFile(int peerfd, const char * filename)
{
    train_t train;
    memset(&train, 0, sizeof(train));
    //3.1 先发文件名给对端
    train.length = strlen(filename);
    strncpy(train.data, filename, train.length);
    int ret = sendn(peerfd, &train, 4 + train.length);

    //3.2 先读取server本地的文件
    char sendBuf[SENDBUFSIZE] = {0};
    int fileFd = open(filename, O_RDWR);
    int nread = read(fileFd, sendBuf, SENDBUFSIZE);
    //3.3 发送文件内容
    memset(&train, 0, sizeof(train));
    train.length = nread;
    strncpy(train.data, sendBuf, train.length);
    ret = sendn(peerfd, &train, 4 + train.length);
    printf("child send %d bytes.\n", ret);

    close(fileFd);
}



//从peerfd接受 filename文件
void recvFile(int peerfd, const char * filename){
	//3. 下载文件
    
    //①接收文件名大小   从clientfd接受4字节数据到length
	int length=0;
	int ret = recv(peerfd, &length, 4, MSG_WAITALL);
	printf(">>1. filename length:%d\n", length);
	
    //②根据文件名大小接收文件名
	char recvBuff[BUFSIZE] = {0};//应用层接收缓冲区
	ret = recv(peerfd, recvBuff, length, MSG_WAITALL);
	printf(">>2. recv filename:%s\n", recvBuff);

	//在客户端本地，创建一个文件 文件名为recvBuff中的内容
    char pathname[BUFSIZE]={0};
    strcat(pathname,"file/");
    strcpy(pathname,recvBuff);
	int fileFd = open(pathname, O_CREAT|O_RDWR, 0644);

	//③接收文件内容大小  从peerfd 接收4字节数据到length
	ret = recv(peerfd, &length, 4, MSG_WAITALL);
	printf(">>3. filecontent length:%d\n", length);

    //④根据文件内容大小接收文件内容
	memset(recvBuff, 0, sizeof(recvBuff));
	ret = recv(peerfd, recvBuff, length, MSG_WAITALL);
	printf(">>4. filecontent:%s\n", recvBuff);

	//3.4 写入本地
	ret = write(fileFd, recvBuff, ret);
}

