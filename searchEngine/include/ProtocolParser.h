/**
 * Project SearchEngine
 */


#ifndef _PROTOCOLPARSER_H
#define _PROTOCOLPARSER_H
#include "TcpConnection.h"
#include "WebPageSearcher.h"
#include "KeyRecommander.h"
#include<functional>
#include<string>
#include<iostream>
using std::string;
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TaskType=std::function<void()>;

class ProtocolParser {

public:
ProtocolParser(WebPageQuery& query, CacheManager& manager)
    :_webPageQuery(query)
     ,_cacheManager(manager)
    { std::cout<<"ProtocolParser(WebPageQuery&,CacheManager&)"<<std::endl; }


TaskType operator()(const string&msg,const TcpConnectionPtr &con );



private:
WebPageQuery& _webPageQuery;
CacheManager&  _cacheManager;
};

#endif //_PROTOCOLPARSER_H
