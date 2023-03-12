#define _DEFINE_MY_CONF_  //在定义了宏_CONF_的模块中，会定义全局变量pConf
#define LOGGER_LEVEL LL_WARN 

#include"../include/SingleTon.h"
#include"../include/Configuration.h"
#include"../include/SplitToolCppJieba.h"
#include "../include/simhash/Simhasher.hpp"
#include"../include/PageLib.h"
#include"../include/PageLibPreprocessor.h"

#include"../include/SingleTon.h"

#include<string>
#include<iomanip>

using std::string;
using namespace simhash;

extern Configuration* pConf;

/* Configuration* pConf=SingleTon<Configuration>::getInstance(); */  
extern Configuration* pConf;

string CONFPATH="../conf/file_path.conf";  //配置文件路径 
extern string CONFPATH;  //配置文件路径 

void initConf(); 
void testjieba(SplitTool&JB);

int main(){
    initConf(); 
    SplitToolCppJieba JB;//全局对象 结巴对象
    testjieba(JB);
    //创建原始的网页库../data/ripepage.dat
#if 0
    {
        cout<<endl<<">>>begin create a PageLib obj"<<endl;
        PageLib lib1;
        clock_t start,end;
        start=clock();
        lib1.create();
        end=clock();
        cout<<"handle xml :time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）


        start=clock();
        lib1.store();
        end=clock();
        cout<<"original webpage storeOnDisk:time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
        cout<<">>>endof a PageLib obj"<<endl;

    }
    //对网页库进行去重生成去重后的网页库，偏移库，倒排索引库
    {
        cout<<endl<<"===begin create a PageLibPreprocessor obj"<<endl;
        PageLibPreprocessor lib2(JB);
        cout<<"_____endof create a PageLibPreprocessor lib2(JB);"<<endl;

        clock_t start,end;
        cout<<endl<<">>>begin  lib2.readInfoFromFile();"<<endl;
        start=clock();
        lib2.readInfoFromFile();
        end=clock();
        cout<<"readInfoFromFile: time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
        cout<<"_____endof lib2.readInfoFromFile();"<<endl;

        cout<<endl<<">>>begin lib2.cutRedundantPage();"<<endl;
        start=clock();
        lib2.cutRedundantPage();
        end=clock();
        cout<<"cutRedundantPage: time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
        cout<<"____endof lib2.cutRedundantPage();"<<endl;

        cout<<endl<<">>>begin lib2.bulidInvertIndexMap();"<<endl;
        start=clock();
        lib2.bulidInvertIndexMap();
        end=clock();
        cout<<"buildInvertIndexMap :time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
        cout<<"___endof lib2.bulidInvertIndexMap();"<<endl;

        cout<<endl<<">>>begin lib2.storeOnDisk();"<<endl;
        start=clock();
        lib2.storeOnDisk();
        end=clock();
        cout<<"storeOnDisk:time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
        cout<<"_____endof lib2.storeOnDisk();"<<endl;
        cout<<"=====endof a PageLibPreprocessor obj"<<endl;


    }
    /* cout<<"pid:"<<getpid()<<endl; */
    return 0;
}

void initConf(){
    auto ptr=SingleTon<Configuration>::getInstance();
    ptr->init(CONFPATH);
    cout<<">>>conf has init successfully"<<endl;

    cout<<">>>begin test confguration's Map"<<endl;
    /* pConf->testConfMap(); */
    ptr->testConfMap();
    cout<<">>>endof test confguration's Map"<<endl;
#if 0
    cout<<"begin test StopSet"<<endl;
    pConf->testStopSet();
#endif 
}


void testjieba(SplitTool&JB){
    cout<<">>>begin test jieba"<<endl;
    vector <string>t=JB.cut("小明去了清华大学小王去了北京大学");
    for(auto &it :t){
        cout<<it<<endl;
    }
    cout<<">>>end test jieba"<<endl;

#if 0
    auto myStopSet=pConf-> getStopWordSet();

    cout<<"sizeof stop set is:"<<myStopSet.size()<<endl;
    for(auto &e:myStopSet){
        cout<<e<<" ";
    }cout<<endl;
#endif 
#endif

}
