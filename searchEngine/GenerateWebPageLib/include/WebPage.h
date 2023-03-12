#pragma once
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include "SplitTool.h"
#include "Configuration.h"

using std::string;
using std::map;
using std::unordered_map;
using std::vector;
using std::set;

class Configuration;
class SplitTool;

class WebPage {
    friend bool operator==(const WebPage & lhs,const WebPage & rhs);//判断两篇文档是否相等
    friend bool operator < (const WebPage & lhs,const WebPage & rhs);//对文档按 Docid 进行排序
public:

    WebPage(string & doc, Configuration &config, SplitTool &jieba);
    int getDocId() {return _docId;}
    string getDoc() {return _doc;}
    string getDocContent() {return _docContent;}
    string getDocUrl() { return _docUrl;}
    string getDocTitle() {return _docTitle;}
    unordered_map<string, int> & getWordsMap() {return _wordsMap;}

private:
    void processDoc();//对格式化文档进行处理
    void creatMap();//生成这篇文章的词频词典_wordsMap 且进行去重处理

private:
    const static int TOPK_NUMBER = 20;
    Configuration _config;
    SplitTool &_jieba;

    string _doc;  //整篇文档， 包含 xml 在内
    
    int _docId; //文档 id
    string _docTitle; //文档标题
    string _docUrl; //文档 URL
    string _docContent; //文档内容

    //string _docSummary; //文档摘要， 需自动生成， 不是固定的
    unordered_map<string,int> _wordsMap; //保存每篇文档的所有词语和词频， 不包括停用词
};
