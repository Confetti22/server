/**
 * Project SearchEngine
 */


#ifndef _CACHEMANAGER_H
#define _CACHEMANAGER_H

#define PAGE_CACHE_PATH "../data/Page_cache.dat"

#include"../include/LRUCache.h"
#include<string>
#include<vector>
using std::vector;
using std::string;


class CacheManager {
public: 


    CacheManager(int size);
    void init(string filename);

    LRUCache& getCache(int idx);

    void updateCache();

private: 
    vector<LRUCache> _caches;
    int _size;
};

#endif //_CACHEMANAGER_H
