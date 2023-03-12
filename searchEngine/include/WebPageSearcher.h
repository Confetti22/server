/**
 * Project SearchEngine
 */
/* #define USE_CACHE */

#ifndef _WEBPAGESEARCHER_H
#define _WEBPAGESEARCHER_H

#include"TcpConnection.h"
#include"Thread.h"  //使用线程局部变量name
#include"WebPageQuery.h"
#include"CacheManager.h"
#include <string>

using std::string;
using TcpConnectionPtr = shared_ptr<TcpConnection>;

class LRUCache;

class WebPageSearcher {
public: 
    WebPageSearcher(const string & sought, const TcpConnectionPtr&con,
                    WebPageQuery&query, CacheManager&manager)
        :_sought(sought)
         ,_conn(con)
         ,_webPageQuery(query)
         ,_cacheManager(manager)
    {
    }

    void doQuery();
private: 
    string _sought;
    TcpConnectionPtr _conn;
    WebPageQuery &_webPageQuery;
    CacheManager &_cacheManager;
};

#endif //_WEBPAGESEARCHER_H
