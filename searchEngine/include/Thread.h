#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>
#include <string.h>

using std::string;
using std::function;

using ThreadCallback = function<void()>;

//线程局部变量name的声明
extern __thread const char *name;

class Thread
{
public:
    Thread(ThreadCallback &&cb,const string&name);
    virtual ~Thread();
    void start();
    void join();
private:
    static void *threadFunc(void *arg);

private:
    pthread_t _thid;
    string _name;
    bool _isRunning;
    ThreadCallback _cb;
};

#endif
