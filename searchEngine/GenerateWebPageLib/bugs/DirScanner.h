#ifndef _DIRSCANNER_H
#define _DIRSCANNER_H

#include"../include/head.h"
#include<func.h>
#include <dirent.h>

class DirScanner {
public: 
    DirScanner();//构造函数
    vector<string>& getFiles();//返回文件路径的引用
    void traverse(string dir);//获取目录dir下的所有文件
    void operator()(string dir);//重载函数调用运算符，调用traverse函数
private: 
    vector<string> _files;//存放每个语料文件的相对路径 “../data/xxx/xxx.xml”
};

DirScanner::DirScanner()
{
    _files.reserve(128);
}
vector<string>& DirScanner::getFiles()
{
    return _files;

}
void DirScanner::traverse(string dir)
{
    //opendir
    DIR *pdir = opendir(dir.c_str());
    ERROR_CHECK(pdir, NULL, "opendir");

    //change current working dirctory
    chdir(dir.c_str()); 

    // 读写目录项
    struct dirent* pdirent;
    while ((pdirent = readdir(pdir)) != NULL) {
        if(pdirent->d_name[0]!='.')//.  .. 不会计入
            _files.push_back(dir+"/"+pdirent->d_name);
    }
    closedir(pdir);

}
void DirScanner::operator()(string dir)
{
    traverse(dir);
}
#endif //_DIRSCANNER_H
