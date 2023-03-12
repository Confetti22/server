
#ifndef _WEBPAGEQUERY_H
#define _WEBPAGEQUERY_H

#include"head.h"
#include"Configuration.h"
#include "WebPage.h"

class SpliTool;

class WebPageQuery {
public: 

    WebPageQuery(SplitTool&cutter,Configuration& conf=*pConf);
    //根据配置信息，读取newoffset.dat invertIndexTable中的内容
void readInfoFromFile();

//根据查询单词获取基准向量
vector<double>getQueryWordsWeightVec(vector<string>&queryWords);
    
string doQuery(string key);

private: 
    unordered_map<int, pair<int, int>> _offsetlib;
    unordered_map<string,vector<pair<int, double>>> _invertIndexLib;
    Configuration& _conf;
    SplitTool &_wordCutTool;
};

#endif //_WEBPAGEQUERY_H
