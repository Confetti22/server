#include "../include/EventLoop.h"
#include "../include/Acceptor.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

using std::endl;
using std::cout;

EventLoop::EventLoop(Acceptor &acceptor,TimerManager&timer,CacheManager&manager)
    : _epfd(createEpollFd())
    , _evfd(createEventFd())
    , _timer(timer)
    , _cacheManager(manager)
    , _acceptor(acceptor) 
    , _isLooping(false)
      , _evtList(1024)
{
    _timer.setTimerfd(_timer._initsec,_timer._perisec);

    addEpollReadFd(_acceptor.fd());//监听listenfd
    addEpollReadFd(_evfd);//将eventfd放在红黑树上进行监听
    addEpollReadFd(_timer._timerfd);
    addEpollReadFd(_timer._toinitCachefd);
}

EventLoop::~EventLoop()
{
    if(_epfd)
    {
        close(_epfd);
    }

    if(_evfd)
    {
        close(_evfd);
    }

}

void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::runInLoop(Functor &&cb)
{
    {
        MutexLockGuard autoLock(_mutex);
        _pendingsCb.push_back(std::move(cb));
    }

    wakeup();
}

void EventLoop::setConnectionCallback(TcpConnectionCallback &&cb)
{
    _onConectionCb = std::move(cb);
}
void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    _onMessageCb = std::move(cb);

}
void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    _onCloseCb = std::move(cb);

}
void EventLoop::waitEpollFd()
{
    int nready = -1;


    do
    {
        nready = epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), 3000);
    }while(-1 == nready && errno == EINTR);

    if(-1 == nready)
    {
        perror("epoll_wait nready == -1");
        return;
    }
    else if(0 == nready)
    {
        cout << ">>epoll_wait timeout" << endl;
    }
    else 
    {
        //可以解决扩容的问题，连接请求超过1024
        if(nready == (int)_evtList.size())
        {
            _evtList.resize(2 * nready);
        }

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = _evtList[idx].data.fd;
            auto event=_evtList[idx].events;
            if(fd == _acceptor.fd() && event&EPOLLIN)//有新的连接
            {
                handleNewConnection();//处理新的连接请求
            }
            else if(fd == _evfd &&event&EPOLLIN)
            {
                cout<<"#######_evfd ready!########"<<endl;
                handleRead();
                //执行所有的任务
                doPendingFunctors();
                cout<<"#######_evfd handled!######"<<endl;
            }
            else if(fd==_timer._timerfd && event&EPOLLIN)
            {

               cout<<"~~~~~~~~~_timerfd ready!~~~~~~"<<endl;


                _timer.readTimerfd();

                pid_t pid=fork();
                if(pid==0){
                    cout<<">>>>>>>>in child"<<getpid()<<endl;
                    //子进程，执行同步的上半部分
                    if(_timer._cb)
                    {
                        _timer._cb();//执行任务
                    }

                    //子进程完成任务后，通过_toinitCachefd通知父进程执行
                    //同步的下半部分，并且回收子进程的资源
                    _timer.writeEventfd();
                    cout<<">>>>>>>>child:"<<getpid()<<"ready to leave"<<endl;
                    exit(0);

                }
                cout<<"RETURN to parent"<<getpid()<<endl;
                cout<<"~~~~~~~_timerfd handled!~~~~~~~"<<endl;

            }
            else if(fd==_timer._toinitCachefd &&event&EPOLLIN)
            {
                cout<<"--------_toinitCachefd  ready--------"<<endl;
                _timer.readEventfd();

                //父进程回收子进程资源
                wait(nullptr); 

                //父进程进行同步的后半部分操作
                _cacheManager.init("../data/Page_cache.dat");
                cout<<"---------_toinitCachefd handled!---------"<<endl;
                

            }
            else//老的连接上有数据发过来了
            {
                if(_evtList[idx].events & EPOLLIN)//并且是读事件
                {
                cout<<"####### a peerfd ready!########"<<endl;
                    handleMessage(fd);//进行正常业务逻辑,进行数据的收发
                cout<<"####### a peerfd handled!########"<<endl;
                }

            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerfd = _acceptor.accept();
    if(peerfd < 0)
    {
        perror("peerfd");
    }
    addEpollReadFd(peerfd);

    //创建Tcp连接
    TcpConnectionPtr con(new TcpConnection(peerfd, this));
    //注册三个事件
    con->setConnectionCallback(_onConectionCb);
    con->setMessageCallback(_onMessageCb);
    con->setCloseCallback(_onCloseCb);

    _conns.insert(std::make_pair(peerfd, con));
    con->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    if(it != _conns.end())
    {
        bool flag = it->second->isClosed();
        if(flag)
        {
            //处理连接断开
            it->second->handleCloseCallback();
            delEpollReadFd(fd);
            _conns.erase(it);
        }
        else
        {
            //处理消息到达的事件
            it->second->handleMessageCallback();
        }
    }
    else
    {
        cout << "该连接不存在" << endl;
            //处理连接断开
            it->second->handleCloseCallback();
            delEpollReadFd(fd);
            _conns.erase(it);
    }
}

//执行所有的任务
void EventLoop::doPendingFunctors()
{
    vector<Functor> tmp;
    {
        //粒度小一些
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendingsCb);
    }

    /* for(auto &cb : _pendingsCb) */
    for(auto &cb : tmp)
    {
        cb();
    }
}
void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = read(_evfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = write(_evfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }

}
int EventLoop::createEventFd()
{
    int fd = eventfd(0, 0);
    if(fd < 0)
    {
        perror("eventfd");
        return -1;
    }

    return fd;
}

int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);
    if(fd < 0)
    {
        perror("epoll_create1");
    }
    return fd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl add");
        return;
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    evt.events = EPOLLIN;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl del");
        return;
    }
}
