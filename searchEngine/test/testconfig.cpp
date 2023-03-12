#define _DEFINE_MY_CONF_  //在定义了宏_CONF_的模块中，会定义全局变量pConf
#include"../include/SingleTon.h"
#include"../include/Configuration.h"
#include"../include/SplitToolCppJieba.h"
#include<string>
using std::string;

string CONFPATH="../conf/file_path.conf";   


void testjieba(SplitToolCppJieba&JB);

int main(){

    auto pConf=SingleTon<Configuration>::getInstance();
    pConf->init(CONFPATH);
    cout<<">>>conf has init successfully"<<endl;

    /* cout<<">>>begin test conMap"<<endl; */
    /* pConf->testConfMap(); */

    /* cout<<"begin test StopSet"<<endl; */
    /* pConf->testStopSet(); */

    cout<<"begin test jieba"<<endl;
    SplitToolCppJieba JB;//全局对象 结巴对象
    testjieba(JB);
}



void testjieba(SplitToolCppJieba&JB){
    cout<<">>>begin test jieba.cut()"<<endl;
    vector <string>t=JB.cut("小明去了清华大学小王去了北京大学");
    for(auto &it :t){
        cout<<it<<",";
    }
    cout<<">>>end test jieba.cut()"<<endl;

    cout<<">>>begin test jieba()"<<endl;
    vector <string>t2=JB("小明去了清华大学小王去了北京大学");
    for(auto &it :t2){
        cout<<it<<",";
    }
    cout<<">>>end test jieba()"<<endl;

#if 0
    auto myStopSet=pConf-> getStopWordSet();

    cout<<"sizeof stop set is:"<<myStopSet.size()<<endl;
    for(auto &e:myStopSet){
        cout<<e<<" ";
    }cout<<endl;
#endif 

}
