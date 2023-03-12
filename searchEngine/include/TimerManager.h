#ifndef __TimerManager_H__
#define __TimerManager_H__

#include <functional>
using std::function;

class TimerManager
{
    friend class EventLoop;
    using TimerCallback = function<void()>;
public:
    TimerManager(int initsec, int perisec, TimerCallback &&cb);
    ~TimerManager();
    void start();
    void stop();

private:
    int createEventfd();
    void readEventfd();
    void writeEventfd();

    int createTimerfd();
    void readTimerfd();
    void setTimerfd(int initsec, int perisec);

public:
    int _timerfd;
    int _toinitCachefd;
private:
    int _initsec;
    int _perisec;
    TimerCallback _cb;
    bool _isStarted;
};

#endif
