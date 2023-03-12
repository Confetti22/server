#include "../include/TimerManager.h"
#include<func.h>
#include <time.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <poll.h>

#include <iostream>

using std::cout;
using std::endl;

TimerManager::TimerManager(int initsec, int perisec, TimerCallback &&cb)
    : _timerfd(createTimerfd())
    , _toinitCachefd(createEventfd())
    , _initsec(initsec)
    , _perisec(perisec)
    , _cb(std::move(cb))
      , _isStarted(false)
{

}

TimerManager::~TimerManager()
{
    if(_timerfd)
    {
        //设置停止定时器
        setTimerfd(0, 0);
        close(_timerfd);
    }
}

int TimerManager::createEventfd(){
    int fd=eventfd(0,0);
    if(fd<0){
        perror("eventfd");
        return -1;
    }
    return fd;

}
void TimerManager::readEventfd(){
    uint64_t one = 1;
    int ret = read(_toinitCachefd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }

}
void TimerManager::writeEventfd(){
    uint64_t one = 1;
    int ret = write(_toinitCachefd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write");
        return;
    }
    cout<<"The child:"<<getpid()<<"has notified the parent to do the rest job"<<endl;

}
int TimerManager::createTimerfd()
{
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if(fd < 0)
    {
        perror("TimerFd");
        return -1;
    }

    cout<<"timerfd:"<<fd<<"has been settime"<<endl;
    return fd;
}


void TimerManager::readTimerfd()
{
    uint64_t one = 1;
    int ret = read(_timerfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }
}


void TimerManager::setTimerfd(int initsec, int perisec)
{
    struct itimerspec value;
    value.it_value.tv_sec = initsec;
    value.it_value.tv_nsec = 0;

    value.it_interval.tv_sec = perisec;
    value.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(_timerfd, 0, &value, nullptr);
    if(ret < 0)
    {
        perror("timerfd_settime");
        return ;
    }
    cout<<"timerfd:"<<_timerfd<<"has been settime"<<endl;
    _isStarted=true;
}


void TimerManager::stop()
{
    _isStarted = false;
    setTimerfd(0, 0);
}

