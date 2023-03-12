#include "../include/WebPage.h"

class Configuration;
class SplitTool;

WebPage::WebPage(string & doc, Configuration &config, SplitTool &jieba)
    :_config(config)
    ,_jieba(jieba)
     ,_doc(doc)
{
    //从格式化的doc中提取信息到本类的数据成员中
    processDoc();
    /* _topWords.reserve(TOPK_NUMBER); */
}

void WebPage::processDoc(){
    auto begin=_doc.find("<docid>");
    auto end=_doc.find("</docid>");
    string id(_doc,begin+7,end-begin-7);
    _docId=atoi(id.c_str());

    begin=_doc.find("<url>");
    end=_doc.find("</url>");
    _docUrl=string(_doc,begin+5,end-begin-5);

    begin=_doc.find("<title>");
    end=_doc.find("</title>");
    _docTitle=string(_doc,begin+7,end-begin-7);

    begin=_doc.find("<content>");
    end=_doc.find("</content>");
    _docContent=string(_doc,begin+9,end-begin-9);

     creatMap();
}

void WebPage::creatMap(){
    //调用分词工具jieba生成vector<string>
    //然后统计词频，建立_wordsMap
    vector<string> word=_jieba.cut(_docContent);
    for(auto it=word.begin();it!=word.end();++it){
        _wordsMap[*it]++;
    }

    //给_wordsMap去重
    unordered_set<string> stopWord = _config.getStopWordSet();
    for(auto it=stopWord.begin();it!=stopWord.end();++it){
        auto p=_wordsMap.find(*it);
        if(p!=_wordsMap.end()){
            _wordsMap.erase(p);
        }
    }
}



