#include "../include/Configuration.h"
#include "../include/SingleTon.h"
#include <fstream>
#include <sstream>

using std::istringstream;
using std::ifstream;


void Configuration:: init(const string&filepath){
    _filepath=filepath;
     Configuration::buildConfMap();
      Configuration::buildStopWord();
}


//从filepath中读取配置文件 并建立 map
void Configuration::buildConfMap()
{
    //打开配置文件filepath并绑定到流对象ifs
    ifstream ifs(_filepath);
	if (!ifs.good()){
        std::cerr << "ifsrtream open file error" << endl;
		return;
	}

    string line;
    while (getline(ifs, line))
    {
        istringstream is(line);
        string key;
        string value;
        is>>key>>value;
        _configs[key]=value;
    }
}

void Configuration::buildStopWord()
{
    string en="en_file_path";
    string zh="zh_file_path";
    readStopWord(_configs[en]);
    readStopWord(_configs[zh]);
}


void Configuration:: readStopWord(const string& filepath)
{

    //创建一个文件流对象infile, 并将他绑定到给定的文件filename
    std::ifstream infile;
    infile.open(filepath);
    if(!infile.good()){
        std::cerr<<filepath<<"open error"<<endl;
        return ;
    }
/**********创建_dict***********/   
    //不断地从infile中读出一行数据
   string line;
   while(getline(infile,line)){
       //从一行数据line中拆解出一个个单词
       std::istringstream is(line);
       string word;
       while(is>>word){
           _stopWordSet.insert(word);
       }//while(is>>word)
   }
    //关闭文件流
    infile.close();


}
map<string,string> &Configuration::getConfigMap()
{
    return _configs;
}
 unordered_set<string>& Configuration:: getStopWordSet()
{
    return _stopWordSet;
}


void Configuration::testConfMap(){
    cout<<"sizeof stopset:"<<_configs.size()<<endl;
    for(auto it = _configs.begin(); it !=_configs.end(); it++)
    {
        cout << (*it).first.c_str() << ' ' << (*it).second << endl;
    }

}
void Configuration::testStopSet(){
    cout<<"sizeof stopset:"<<_stopWordSet.size()<<endl;

    auto it=_stopWordSet.find("可以");
    if(it!=_stopWordSet.end())
    cout<<"find result:"<<*it<<endl;

         it=_stopWordSet.find("albeit");
    if(it!=_stopWordSet.end())
    cout<<"find result:"<<*it<<endl;

    for(auto &e:_stopWordSet){
        cout<<e<<" ";
    }cout<<endl;

}
