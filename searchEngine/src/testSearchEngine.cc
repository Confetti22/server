#define _DEFINE_MY_CONF_  //在定义了宏_CONF_的模块中，会定义全局变量pConf
#define LOGGER_LEVEL LL_WARN 

#include"../include/SingleTon.h"
#include"../include/Configuration.h"
#include"../include/SplitToolCppJieba.h"
#include "../include/simhash/Simhasher.hpp"
#include"../include/SearchEngineServer.h"


#include<string>
#include<iomanip>

using std::string;
using namespace simhash;



string CONFPATH="../conf/file_path.conf";  //配置文件路径 

void initConf(); 

int main()
{
    initConf();
    SplitToolCppJieba JB;//全局对象 结巴对象
    SearchEngineServer server1(4,10,"127.0.0.1",8888,JB);
    server1.start();

    return 0;
}




void initConf(){
    auto ptr=SingleTon<Configuration>::getInstance();
    ptr->init(CONFPATH);
    cout<<">>>conf has init successfully"<<endl;

}
