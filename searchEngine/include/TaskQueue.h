#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include "MutexLock.h"
#include "Condition.h"
#include <queue>
#include <functional>

using std::queue;
using std::function;

//任务的类型是一个function<void()>
using Task = function<void()>;

class TaskQueue
{
public:
    TaskQueue(size_t sz);
    ~TaskQueue();
    bool empty() const;
    bool full() const;
    /* void push(const Task &value); */
    void push(Task &&value);
    Task pop();

    void wakeup();

private:
    size_t _queSize;
    queue<Task> _que;
    MutexLock _mutex;
    Condition _notEmpty;
    Condition _notFull;
    bool _flag;

};

#endif
