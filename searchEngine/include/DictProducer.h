#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H
#include"head.h"

class SplitTool;
class DictProducer {
public: 

    DictProducer(const string &dir, SplitTool* tool);//构造函数，专门针对中文
    DictProducer(const string &dir);

    void buildEnDict();

    void buildCnDict();

    void createIndex();

    void storeDict(const char* filepath);//将内存中的词典写入文件
private: 
    std::vector<string> _files;//语料库文件的绝对路径集合
    vector<pair<string,int>> _dict; //词典<word, frequency>
    map<string,set<int>> _index; //词典索引<,出现的行号>
    SplitTool* _cuttor;//分词工具
};

#endif //_DICTPRODUCER_H
