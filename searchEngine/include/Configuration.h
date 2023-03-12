#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include"SingleTon.h"
#include <iostream>
#include <map>
#include <unordered_set>
#include <string>
//使用了单例模式
using std::cout;
using std::endl;
using std::map;
using std::string;

using std::unordered_set;

//配置文件路径
/* string CONFPATH="../conf/file_path.conf"; */
//Configuration类的使用方法：
//创建一个Configuration类的单例对象，getInstance()
//然后调用init方法后，即可使用Configuration类中的数据

class Configuration{
public:
    Configuration(){cout<<"Configuration()"<<endl;}
    /* ~Configuration(){cout<<"~Configuration()"<<endl;} */
    //init()方法会调用以下两个函数
    //建立_configs 和_stopWordSet
    void init(const string &filepath="../conf/file_path.conf");

    //读取配置文件filepath,并建立_configs
    void buildConfMap();
    //读取停词文件filepath， 并建立_stopwordSet
    void readStopWord(const string& filepath);  
    void buildStopWord();
    //获取configMap
    //
    map<string,string> &getConfigMap();
    //获取StopwordSet
    unordered_set<string>& getStopWordSet();

    void setConfFilePath(const string&filepath);
    void testConfMap();
    void testStopSet();

private:
    string _filepath; //配置文件路径
    map<string,string> _configs;//储存了每一个配置的名字和它的相对路径
    unordered_set<string> _stopWordSet;//这里储存了处理好的停用词数据，无序
};



#ifdef _DEFINE_MY_CONF_
Configuration* pConf=SingleTon<Configuration>::getInstance();  
/* pConf->init(CONFPATH); */
//在定义了_CONF_宏的main函数中，pConf全局变量pConf
#else
extern Configuration* pConf;
//在其他模块中的pConf是一个外部变量
#endif




#endif
