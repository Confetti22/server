#include"../include/WebPageSearcher.h"
#define USE_CACHE



#ifdef USE_CACHE
    /*********************子线程开始任务*****************************/
    void WebPageSearcher::doQuery()
    {

        cout<<"begin WebPageSearcher:doQuery()"<<endl;
        //①先在缓存中查找_sought
        int threadNo=atoi(name);
        LRUCache&cache=_cacheManager.getCache(threadNo);
        cout<<"threadNo="<<threadNo<<endl;
        cout<<"Cache 的数据："<<endl;
        cache.printCache();

        string result=cache.get(_sought);
        if(result.size()==0)
        {
            cout<<"Cache 未命中,创建一个WebPageQuery对象进行doQuery()"<<endl;

            //②缓存中没有时，需要通过WebPageQuery对象进行搜索任务
            result=_webPageQuery.doQuery(_sought);
            //判断webPageQuery.doQuery(_sought)的结果
            //结果为空时， 则查询结果为空，令result="No WebPage match!"
            //结果不为空时，需要将查询结果写入当前缓存
            if(result.size()==0){
                result="doQuery()结果：No WebPage match!";
            }else{
                cache.addElement(_sought, result);
                cout<<"doQuery()结果：new key-value added to "<<"number:"<<threadNo<<"Cache"<<endl;
            }
        }else{
            cout<<"Cache 命中"<<endl;
        }

        //将计算线程处理好的数据交给I/O线程EventLoop，
        //让I/O线程把数据返回给客户端
        _conn->sendInLoop(result);
    }
    /**********************子线程结束任务*************************/
#else


    /*********************子线程开始任务*****************************/
    void WebPageSearcher::doQuery()
    {


            //②缓存中没有时，需要通过WebPageQuery对象进行搜索任务
            cout<<"In WebPageSearcher, _sought="<<_sought<<endl;
            string result=_webPageQuery.doQuery(_sought);
            cout<<"In WebPageSearcher, WebPageQuery::doQuery(),result="<<result<<endl;
            //判断webPageQuery.doQuery(_sought)的结果
            //结果为空时， 则查询结果为空，令result="No WebPage match!"
            //结果不为空时，需要将查询结果写入当前缓存
            if(result.size()==0){
                result="No WebPage match!";
            }

        //将计算线程处理好的数据交给I/O线程EventLoop，
        //让I/O线程把数据返回给客户端
        _conn->sendInLoop(result);
    }
    /**********************子线程结束任务*************************/

#endif
