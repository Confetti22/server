#ifndef _DIRSCANNER_H
#define _DIRSCANNER_H

#include"head.h"
#include<func.h>
#include <dirent.h>

class DirScanner {
public: 
    DirScanner();//构造函数
    vector<string>& getFiles();//返回文件路径的引用
    void traverse(string dir);//获取目录dir下的所有文件
    void operator()(string dir);//重载函数调用运算符，调用traverse函数
private: 
    vector<string> _files;//存放每个语料文件的相对路径 “../data/...”
};

#endif //_DIRSCANNER_H
