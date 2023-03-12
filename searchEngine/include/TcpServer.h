#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"

class CacheManager;
class TimerManager;

class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port,
              CacheManager&cacheManager,TimerManager&timerManager)
    : _cacheManager(cacheManager)
    , _timerManager(timerManager)
    , _acceptor(ip, port)
    , _loop(_acceptor,timerManager,cacheManager)
    {

    }

    void start()
    {
        _acceptor.ready();
        _loop.loop();
    }

    void stop()
    {
        _loop.unloop();
    }

    void setAllCallback(TcpConnectionCallback &&onConnection
                        , TcpConnectionCallback &&onMessage
                        , TcpConnectionCallback &&onClose)
    {
        _loop.setConnectionCallback(std::move(onConnection));
        _loop.setMessageCallback(std::move(onMessage));
        _loop.setCloseCallback(std::move(onClose));
    }

    ~TcpServer()
    {

    }

private:
    CacheManager& _cacheManager;
    TimerManager& _timerManager;
    Acceptor _acceptor;
    EventLoop _loop;

};

#endif
