#ifndef _PAGELIBPREPROCESSOR_H
#define _PAGELIBPREPROCESSOR_H

#include"head.h"
#include"Configuration.h"
#include"WebPage.h"
class WebPage;
class SplitTool;

class PageLibPreprocessor {
public: 
    PageLibPreprocessor(SplitTool&cutter,Configuration&conf=*pConf)
        :_conf(conf),
        _wordCutter(cutter)
    {
        cout<<"PageLibPreprocessor(Configuration&)"<<endl;
    }
//根据配置信息读取ripepage.dat 和offset.dat的内容
//建立_pageList 
void readInfoFromFile();

//根据_pageList中的信息进行去重
void cutRedundantPage();

//创建倒排索引_invertIndexTable
void bulidInvertIndexMap();
    
//将去重后的网页库，偏移库和倒排索引库存到磁盘上
void storeOnDisk();
private: 
    vector<WebPage> _pageList;//存放一片片的网页对象
    unordered_map<int ,pair<int ,int>> _offsetLib;//偏移库数据
    unordered_map<string,vector<pair<int ,double>>> _invertIndexLib;//倒排索引库数据
    Configuration& _conf;
    SplitTool& _wordCutter;//分词工具
};

#endif //_PAGELIBPREPROCESSOR_H
