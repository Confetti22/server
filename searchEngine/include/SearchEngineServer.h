#ifndef __SearchEngineServer_H__
#define __SearchEngineServer_H__

#include"Configuration.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include "CacheManager.h"
#include "TimerManager.h"
#include"KeyRecommander.h"
#include"WebPageSearcher.h"
#include"ProtocolParser.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

class SplitTool;

class SearchEngineServer
{
public:
    SearchEngineServer(size_t threadNum, size_t queSize
                       , const string &ip, unsigned short port
                       , SplitTool &cutter
                       , int initsec=20, int perisec=20
                       ,Configuration&conf=*pConf
                      )
        :_conf(conf)
         ,_wordCutter(cutter)
         , _pool(threadNum, queSize)
         , _cacheManager(threadNum)
         , _timerManager(initsec, perisec, (std::bind(&CacheManager::updateCache,&_cacheManager)))
         , _webPageQuery(cutter,conf)
         , _server(ip, port,_cacheManager,_timerManager)
         , _protocolParser(_webPageQuery, _cacheManager)
    {

    }




    void doTaskThread(const string &msg, const TcpConnectionPtr &con)
    {

        //_msg
        //处理_msg,进行相应的编解码、计算
        //处理完成之后，消息是需要发出去的,需要一个TCP连接
        //消息的接收与发送此时都是Reactor/EventLoop
        //
        //涉及到计算线程（线程池）与IO线程（EventLoop/Reactor）进行通信
        con->sendInLoop(msg);
        //消息要从TcpConnection对象发送给EventLoop进行发送
        //此时TcpConnection必须要知道EventLoop的存在(在TcpConnection中添加
        //EventLoop的引用或者指针)
        //
    }



    void start()
    {
        _pool.start();
        //void(const TcpConnectionPtr &)
        //TcpConnectionPtr con;
        //void setAllCallback(TcpConnectionCallback &&onConnection
        using namespace std::placeholders;

        //bind可以改变函数的形态，函数的返回类型是改不了的
        //只能改变函数参数的个数的，也不能改变函数参数的类型
        //只能减少绑定的函数的参数个数，但是不能增加绑定函数的参数个数
        //bind绑定函数之后，bind的返回类型肯定是一个右值
        //
        //bind默认情况是进行值传递,如果想用引用传递,std::cref/std::ref,
        //想进行地址传递, 要对变量进行取地址
        _server.setAllCallback(std::bind(&SearchEngineServer::onConnection, this,_1),
                               std::bind(&SearchEngineServer::onMessage, this, _1)
                               , std::bind(&SearchEngineServer::onClose, this, _1));
        _server.start();
    }

    void stop()
    {
        _pool.stop();
        _server.stop();
    }

    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }

    void onMessage(const TcpConnectionPtr &con)
    {
        //实现了回显服务
        string msg =  con->receive();
        cout << "recv from client msg : " << msg << endl;

        //利用_protocolParser解析协议类型后，生成对应类型的任务，
        //即把对应的任务处理函数bind成function<void()>的形式

        _pool.addTask(_protocolParser(msg,con));
    }

    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }

public:
    Configuration& _conf;
    SplitTool& _wordCutter;
    ThreadPool _pool;
    CacheManager _cacheManager;
    TimerManager _timerManager;
    WebPageQuery _webPageQuery;
    /* WebPageSearcher _webSearcher; */
    /* KeyRecommander _keyRecommander; */
    TcpServer _server;
    ProtocolParser _protocolParser;
};
#endif
