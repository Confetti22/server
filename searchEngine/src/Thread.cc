#include "../include/Thread.h"
#include <stdio.h>

__thread const char *name = "current_thread";

Thread::Thread(ThreadCallback &&cb, const string &name)
/* Thread::Thread(ThreadCallback &&cb) */
: _thid(0)
, _name(name)
, _isRunning(false)
, _cb(std::move(cb))
{
}

Thread::~Thread()
{
    if(_isRunning)
    {
        pthread_detach(_thid);
    }
}

void Thread::start()
{
    int ret = pthread_create(&_thid, nullptr, threadFunc, this);
    if(ret)
    {
        perror("pthread_create");
        return;
    }

    _isRunning = true;
}

void Thread::join()
{
    if(_isRunning)
    {
        pthread_join(_thid, nullptr);
        _isRunning = false;
    }
}

void *Thread::threadFunc(void *arg)
{
    Thread *pth = static_cast<Thread *>(arg);

    //给线程局部变量name赋值为_name.c_str()
    name = pth->_name.c_str();//从string转换为const char *
    if(pth)
    {
        pth->_cb();//回调函数
    }

    return nullptr;
}
