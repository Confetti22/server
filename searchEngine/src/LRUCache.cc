#include"../include/LRUCache.h"

#if 0
void  printCache(list<pair<string,string>>&cache){
    cout<<"_lruList.size()="<<cache.size()<<endl;
    for(auto e:cache){
        cout<<e.first<<","<<endl;
    }
}
#endif 

void LRUCache:: printCache(){
    cout<<"_lruList.size()="<<_lruList.size()<<endl;
    for(auto e:_lruList){
        cout<<e.first<<","<<endl;
    }
}


using iterator= std::list<pair<string,string>>::iterator;
void printMap(unordered_map<string,iterator>hashMap){
    cout<<"_hashMap.size()="<<hashMap.size()<<endl;
    for(auto e:hashMap){
        cout<<e.first<<","<<endl;
        cout<<"it's iterator in list:"<<(*(e.second)).first<<endl;
    }
}

//read LRUcachefor key 
//return key's value,   empty string if key doesn't exit
string LRUCache::get(string key) {
    auto search=_hashMap.find(key);
    if(search!=_hashMap.end()){
        //cache中时，将该结点（ptr所指结点）移动到链表LRU_list头
        _lruList.splice(_lruList.begin(),_lruList,search->second);
        _updatesize++;
        return search->second->second;           
    }else{
        return "";
    }
}

void LRUCache::addElement(const string&key,const string&value){
    _updatesize++;

    /* cout<<"before add key_value"<<endl; */
    /* printCache(_lruList); */
    /* printMap(_hashMap); */

    auto search1=_hashMap.find(key);
    if(search1==_hashMap.end()){
        //cache中不存在key
        //将新的键值对插入list队头中，并在map中建立key-->iterator的映射
        _lruList.push_front(pair<string,string>(key,value));
        _hashMap[key]=_lruList.begin();
        _size++;
        if(_size>_capacity){
            //若此时cache已经超过capacity大小
            //则list队尾结点出队，并删除其在hashMap中相应映射
            string key2dele=_lruList.back().first;
            int ret=_hashMap.erase(key2dele);
            if(ret!=1){
                std::cerr<<"ret="<<ret<<endl;
                std::cerr<<"_hashMap.earse("<<key2dele<<")"<<"error"<<endl;
            }
            _lruList.pop_back();
            _size--;
        }

    }else{
        //key已存在，则将该键值对移动到list队头成为热数据
        _lruList.splice(_lruList.begin(),_lruList,search1->second);
    }

    /* cout<<"After add key_value"<<endl; */
    /* printCache(_lruList); */
    /* printMap(_hashMap); */
}



void LRUCache::writeToFile(const string&filename){
    std::ofstream ofs(filename);
    if(!ofs){
        perror("open fail");
        std::cerr<<"open"<<filename<<"fail!"<<std::endl;
        return ;
    }

    for(auto it=_lruList.begin();it!=_lruList.end();it++){
        ofs<<"<key>"<<it->first
            <<"</key><value>"<<it->second
            <<"</value>"<<";";
    }
    cout<<"in mainCahce, size of key_value pair has been  Write To File="<<_lruList.size()<<endl;
    ofs.close();
}


void LRUCache::update(const LRUCache&mainCache){
    _updatesize=0;
    _lruList=mainCache._lruList;
    _hashMap=mainCache._hashMap;
}



