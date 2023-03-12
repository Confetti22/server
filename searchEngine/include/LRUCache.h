/**
 * Project SearchEngine
 */


#ifndef _LRUCACHE_H
#define _LRUCACHE_H
#include"head.h"
class LRUCache{
public:
    friend class CacheManager;
    using iterator= std::list<pair<string,string>>::iterator;

    LRUCache(int capacity=1000)
        :_capacity(capacity) 
         ,_size(0)
    {
    }

    //read LRUcachefor key 
    //return key's value,   empty string if key doesn't exit
    string get(string key);
    void addElement(const string&key,const string&value);

    void writeToFile(const string&filename);
    //拷贝mainCache到当前cache中
    void update(const LRUCache&mainCache);

void  printCache();
public:
    //代表一个更新周期内，本cache中list队头更新的结点数
    int _updatesize=0; //类内初始化为零，每次同步过后都会重新清零

private:
    int _capacity;
    int _size;
    list<pair<string,string>>_lruList;//_缓存中的数据
    unordered_map<string,iterator>_hashMap;//在_hashMap中查找
};
#endif //_LRUCACHE_H
