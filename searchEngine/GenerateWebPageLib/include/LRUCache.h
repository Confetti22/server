/**
 * Project SearchEngine
 */


#ifndef _LRUCACHE_H
#define _LRUCACHE_H

class LRUCache {
public: 
    
/**
 * @param filename
 */
void readFromFile(string& filename);
    
/**
 * @param key
 * @param json
 */
void addRecord(void key, void json);
    
/**
 * @param const LRUCache&
 */
void update(void const LRUCache&);
    
/**
 * @param filename
 */
void writeToFile(string& filename);
private: 
    list<key,json> _resultList;
    unordered_map<key,iterator> _hashmap;
    list<key,json> _pendingUpdateList;
};

#endif //_LRUCACHE_H