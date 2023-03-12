///
/// @file    threadpool.c
/// @author  lemon(haohb13@gmail.com)
/// @date    2022-06-06 11:01:26
///

#include "threadpool.h"
#include"threadfun.h"
extern int epfd;
void queueInit(task_queue_t * que)
{
    if(que) {
        que->pFront = NULL;
        que->pRear = NULL;
        que->queSize = 0;
        que->exitFlag = 0;
        int ret = pthread_mutex_init(&que->mutex, NULL);
        THREAD_ERROR_CHECK(ret, "pthread_mutex_init");

        ret = pthread_cond_init(&que->cond, NULL);
        THREAD_ERROR_CHECK(ret, "pthread_cond_init");
    }
}

void queueDestroy(task_queue_t* que)
{
    if(que) {
        int ret = pthread_mutex_destroy(&que->mutex);
        THREAD_ERROR_CHECK(ret, "pthread_mutex_destroy");

        ret = pthread_cond_destroy(&que->cond);
        THREAD_ERROR_CHECK(ret, "pthread_cond_destroy");
    }
}

int getTaskSize(task_queue_t * que)
{	return que->queSize;	}

int queueIsEmpty(task_queue_t *que)
{	return que->queSize == 0;	}

//入队操作
int  taskEnqueue(task_queue_t* que,int peerfd,task_handler_t pfunc,void *para)
{
    int ret = pthread_mutex_lock(&que->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");

    //构建一个新的任务结点
    task_t * ptask = (task_t*)calloc(1, sizeof(task_t));
    ptask->peerfd = peerfd;
    ptask->arg.peerfd=peerfd;
    ptask->pfunc=pfunc;
    strcpy(ptask->arg.commandpara,(char *)para);
    ptask->pnext = NULL;

    //将新任务结点连接到任务队列队尾
    if(queueIsEmpty(que)) {
        que->pFront = que->pRear = ptask;
    }else {
        que->pRear->pnext = ptask;	
        que->pRear = ptask;
    }
    ++que->queSize;
    printf("main thread has put a new task in taskQue\n");

    ret = pthread_mutex_unlock(&que->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_unlock");

    //通知消费者线程取任务
    ret = pthread_cond_signal(&que->cond);
    return 0;
}

//出队操作
task_t taskDequeue(task_queue_t * que)
{
    int ret = pthread_mutex_lock(&que->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_lock");

    //退出为为false && 任务队列为空时，进入等待状态
    while(!que->exitFlag && queueIsEmpty(que)) {//防止虚假唤醒
        pthread_cond_wait(&que->cond, &que->mutex);
    }

    task_t* pFree=NULL;
    //task_t  peerfd  pfunc  arg  pnext
    task_t dequeNode={-1,NULL,{0,{0}},NULL};

    if(!que->exitFlag) {
        pFree = que->pFront;
        if(getTaskSize(que) > 1) {//要出队的结点不是队列中唯一一个结点
            que->pFront = que->pFront->pnext;
        } else {
            pFree = que->pFront;
            que->pFront = que->pRear = NULL;
        }
        dequeNode=*pFree;
        free(pFree);
        --que->queSize;
    }
    ret = pthread_mutex_unlock(&que->mutex);
    THREAD_ERROR_CHECK(ret, "pthread_mutex_unlock");

    //若设置了exitfalg则返回初始化的dequeNode(peerfd=-1)
    return dequeNode;

}

void queueWakeup(task_queue_t* que) 
{
    que->exitFlag = TASKQUEUE_READY_EXIT_FLAG;
    int ret = pthread_cond_broadcast(&que->cond);
    THREAD_ERROR_CHECK(ret, "pthread_cond_broadcast");
}

void threadpoolInit(thread_pool_t* pthreadpool, int threadNum)
{
    pthreadpool->threadNumber = threadNum;
    pthreadpool->threads = (pthread_t*)calloc(threadNum, sizeof(pthread_t));
    queueInit(&pthreadpool->queue);
}

void threadpoolDestroy(thread_pool_t* pthreadpool)
{
    free(pthreadpool->threads);
    queueDestroy(&pthreadpool->queue);
}

void threadpoolStart(thread_pool_t * pthreadpool)
{
    for(int idx = 0; idx < pthreadpool->threadNumber; ++idx) {
        int ret = pthread_create(&pthreadpool->threads[idx],
                                 NULL, 
                                 threadFunc, 
                                 pthreadpool);
        THREAD_ERROR_CHECK(ret, "pthread_create");
    }
}



//每一个子线程要做的事儿
void * threadFunc(void* arg)
{
    printf("sub thread %ld is runing\n", pthread_self());
    thread_pool_t * pthreadpool = (thread_pool_t*)arg;
    while(1) {
        task_t task = taskDequeue(&pthreadpool->queue);
        if(task.peerfd > 0) {
            //执行该任务对应的任务处理函数
            task.pfunc((void*)&task.arg);

            //子线程处理完任务后，再次把该peerfd添加到主线程的epoll读监听事件中
            if(task.pfunc!=handle_exit){
                //当执行的是关闭用户连接操作时，无需再添加读监听
                EpollAdd(epfd,task.peerfd,EPOLLIN);
            }
        } else {//exitFlag=1 ,子线程退出
            break;
        }
    }
    printf("sub thread %ld is ready to exit\n", pthread_self());
    pthread_exit(0);
}
