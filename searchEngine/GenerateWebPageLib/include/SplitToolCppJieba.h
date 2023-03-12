#ifndef __CPPJIEBA_H__
#define __CPPJIEBA_H__
#include "../include/Configuration.h"
#include "../include/simhash/deps/cppjieba/Jieba.hpp"
#include "../include/SplitTool.h"

class SplitToolCppJieba
:public SplitTool
{

    map<string,string>confMap=pConf->getConfigMap();
    unordered_set<string>stopWords=pConf->getStopWordSet();

public:
    SplitToolCppJieba()
        :_conf(pConf)
         ,_jieba(confMap["DICT_PATH"],confMap["HMM_PATH"]
                 ,confMap["USER_DICT_PATH"])
    {
        cout << "cppjieba construct!" << endl;
    }

    //构造函数


    vector<string>operator()(const string &str){
        vector<string>words;
        _jieba.CutAll(str,words);
        return words;
    }
    //virtual vector<string> cut(const string &sentence)=0;
    vector<string> cut(const string &sentence) override;
private:
    Configuration * _conf;   //配置文件的指针
    cppjieba::Jieba _jieba; //结巴对象
};



#endif
