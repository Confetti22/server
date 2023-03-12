#include "../include/CacheManager.h"
#include"../include/LRUCache.h"
#include"../include/head.h"

CacheManager::CacheManager(int size):_size(size)
    {

        cout<<"CacheManager(int size)"<<endl;
        for(int i=0;i<size;i++){
            _caches.push_back(LRUCache());
        }
        cout<<"befor init(PAGE_CACHE_PATH)"<<endl;
        init(PAGE_CACHE_PATH);
        cout<<"CacheManager has inited!"<<endl;
    };



void CacheManager::init(string filename) {
    cout<<"mainCache begin to read data from disk"<<endl;
    //从磁盘文件filename中读入缓存信息到主cache（1号cache）中
    LRUCache& mainCache=getCache(1);
    cout<<"before read in data, mainCache's size is:"<<mainCache._lruList.size()<<endl;

    std::ifstream infile;
    infile.open(filename);
    if(!infile.good()){
        std::cerr<<filename<<"open error"<<std::endl;
        return ;
    }

    string line;
    int i=1;
    while(getline(infile,line,';')){
        auto keybegin=line.find("<key>");
        auto keyend=line.find("</key>");
        string key(line,keybegin+5,keyend-keybegin-5);
        auto valuebegin=line.find( "<value>");
        auto valueend=line.find("</value>");
        string value(line,valuebegin+7, valueend-valuebegin-7);
        /* cout<<"*keybegin"<<line[keybegin+7]<<" , "<<"*keyend"<<line[keyend]<<endl; */
        /* cout<<"*valuebegin"<<line[valuebegin+7]<<" , "<<"*valueend"<<line[valueend]<<endl; */
        cout<<"In init the mainCache, add the "<<i++<<"th key:"<<key;
        cout<<"value="<<value<<endl;
        mainCache.addElement(key,value);
    }

    cout<<"After read in data, mainCache's size is:"<<mainCache._lruList.size()<<endl;
    infile.close();

    //将主cache块的内容依次拷贝到所有cache块中
    for(size_t i=1;i<_caches.size();i++){
        _caches[i].update(mainCache);
    }

    return;
}




void CacheManager::updateCache() {

    cout<<"begin to sum up data to mainCache"<<endl;
    LRUCache& mainCache=getCache(1);

    cout<<"Before  summing up caches, main cache's size is"<<mainCache._size<<endl;
    //①遍历一遍_cacheList,汇总更新信息到主cache
    for(size_t i=1;i<_caches.size();i++){
        LRUCache&subCache=getCache(i+1);
        int updatsize=subCache._updatesize;
        cout<<"subCache:"<<i+1<<"  updatsize:"<<updatsize<<endl;
        auto it=subCache._lruList.begin();
        for(int k=0;k<updatsize;k++){
            mainCache.addElement(it->first,it->second);
            it++;
        }
    }

    cout<<"After summing up caches, main cache's size is"<<mainCache._size<<endl;

    //②将主cache块的信息存储到磁盘上
    mainCache.writeToFile("../data/Page_cache.dat");
    return;
}

LRUCache& CacheManager::getCache(int idx) {
    return _caches[idx-1];
}
