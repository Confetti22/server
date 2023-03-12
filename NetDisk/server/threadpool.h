///
/// @file    processpool.h
/// @author  lemon(haohb13@gmail.com)
/// @date    2022-06-01 20:20:27
///

#ifndef __WD_THREADPOOL_H__
#define __WD_THREADPOOL_H__

#include "tcp.h"

#include <func.h>

#define MAXCONNS 4096
#define TASKQUEUE_NOT_EXIT_FLAG 0
#define TASKQUEUE_READY_EXIT_FLAG 1
#define MAXCOMMAND 128
typedef void*(*task_handler_t)(void*);

typedef struct{
    int peerfd;
    char commandpara[MAXCOMMAND];
}myarg_t;

//任务结点结构体
typedef struct task_s{
    int peerfd;
    task_handler_t pfunc;
    myarg_t arg;
    struct task_s * pnext;
} task_t;  

//任务队列结构体
typedef struct taskqueue_s{
    //队头，队尾指针，pFront, pRear  队列大小queSize
    task_t * pFront;
    task_t * pRear;
    int queSize;
    //互斥锁mutex 条件变量cond
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    //退出的标志位exitFlag
    int exitFlag;

} task_queue_t;

//进程池
typedef struct {
    pthread_t * threads; //工作线程数组
    int threadNumber;//工作线程数量
    task_queue_t queue;//任务队列
} thread_pool_t, *pThread_pool_t;


/**********任务队列API**************/
void queueInit(task_queue_t *);
void queueDestroy(task_queue_t*);
int queueIsEmpty(task_queue_t *);
int getTaskSize(task_queue_t *);
int  taskEnqueue(task_queue_t* que,int peerfd,void*(*task_handle)(void*),void *para);

//从任务队列中取出头任务结点，若设置了exitFlag,则返回空的结构体
task_t  taskDequeue(task_queue_t*);

//设置任务队列的eixtFlag, 
//并调用signal_broadcast()唤醒所有等待在条件变量上的子线程
void queueWakeup(task_queue_t*);



void * threadFunc(void*);
int transferFile(int peerfd, const char * filename);

/************线程池API**************************/
void threadpoolInit(thread_pool_t*, int);
void threadpoolDestroy(thread_pool_t*);
void threadpoolStart(thread_pool_t * pthreadpool);



//每一个子线程要做的事儿
#endif
