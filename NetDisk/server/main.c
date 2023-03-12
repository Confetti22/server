 ///
 /// @file    main.c
 /// @author  lemon(haohb13@gmail.com)
 /// @date    2022-06-01 20:25:01
 ///
 
#include "threadpool.h"
#include"tcp.h"
#include"uList.h"
#include"threadfun.h"
#define IP "127.0.0.1"
#define PORT 8888

const int THREADNUM= 66; //线程池中线程数量
int epfd;
pthread_mutex_t epfd_mutex;

int exitpipe[2];//用于发送，接受 SIGUSR1 (自定义的退出信号)
thread_pool_t threadpool;//线程池（共享资源）
userList * puList;//用户链表指针

void sigfunc(int num) 
{
	printf("sig %d is coming\n", num);
	//父进程接收到SIGUSER1信号后，
    //通过向exitpiepe写入信息来通知子进程要退出了
	char ch = 1;
	write(exitpipe[1], &ch, 1);
}
 
int main(int argc, char *argv[])
{//ip,port,processNum
	signal(SIGUSR1, sigfunc);//10号信号
	//ARGS_CHECK(argc, 4);
	//int THREADNUM = atoi(argv[3]);
	pipe(exitpipe);//创建退出的管道

	pid_t pid = fork();
	if(pid > 0) {
     /******************************父进程***********************************
      ******************************************************************
      ************父进程只负责捕获SIGUSR1信号，捕获该信号后，
      *********** 会通过向exitpipe写入信息来通知子进程（线程池）退出*******
      ***************************************************************/
		close(exitpipe[0]); //父进程关闭退出管道的读端
		wait(NULL); //等待子进程死亡并为其收尸
		exit(0);
	}

     /******************************子进程***********************************
      ******************************************************************
      ***************************************************************/
	close(exitpipe[1]);//子进程关闭退出管道的写端

	threadpoolInit(&threadpool, THREADNUM);
	threadpoolStart(&threadpool);

     /**************************子进程中的主线程***********************************
      ***************************************************************/

	//2. 创建TCP的监听套接字listenfd
    int listenfd = tcpServerInit(IP,PORT);

	//3. 创建epoll的实例, 并注册(监听)相应文件描述符上的事件
	epfd =  EpollCreate();
	EpollAdd(epfd, listenfd,EPOLLIN);//监听listenfd上的读事件
    EpollAdd(epfd, exitpipe[0],EPOLLIN);//监听exitpipe[0]上的读事件

    //pevtList用于接收从epoll_wait从内核返回的就绪事件队列
	struct epoll_event * pevtList = (struct epoll_event*)
		calloc(MAXCONNS, sizeof(struct epoll_event));
	
	int nready;

    //初始化 子线程互斥访问epfd的互斥锁 epfd_mutex;
    int ret=pthread_mutex_init(&epfd_mutex,NULL);
    THREAD_ERROR_CHECK(ret,"pthread_epfd_mutex_init");

    //初始化一个已连接用户链表
     puList=CreateUserList();
    InitUserList(puList);
/**************************以上为准备工作*************************************/

	while(1) {
		do {
			//4. epoll等待事件就绪
			nready = epoll_wait(epfd, pevtList, MAXCONNS, 5000);
		}while(-1 == nready && errno == EINTR);

		if(0 == nready) {
			printf(">> epoll timeout.\n");
			continue;
		} else if(-1 == nready) {
			perror("epoll_wait");
			return EXIT_FAILURE;
		} else {//nready > 0
			printf("nready=%d\n",nready);
            Handle_events( epfd, pevtList,nready,listenfd);
		}
	}

    
	free(pevtList); 
	close(listenfd);

	close(epfd);
   ret= pthread_mutex_destroy(&epfd_mutex);
    THREAD_ERROR_CHECK(ret,"pthread_epfd_mutex_destory");
	return 0;
}
