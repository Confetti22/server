#include "../include/PageLib.h"
#include "../include/tinyxml2.h"
#include <iostream>
#include <string>
#include<string.h>
#include <regex>
#include <vector>
#include <fstream>
#include<memory.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ofstream;
using namespace tinyxml2;

class Configuration;
class DirScanner;
class FileProcessor;

//有待于放到配置文件中
#define  RIPEPAGE  "ripepage.dat"
#define  RIPE_PATH  "../data/ripepage.dat"
#define  OFFSET  "offset.dat"
#define  OFFSET_PATH  "../data/offset.dat"
#define  DIR_NAME  "../data/m2yuliao"
#define  PARENT_DIR_NAME  "../data/"
//解析一篇xml文章filename
void read(string &filename);

PageLib::PageLib(){}
PageLib::PageLib(Configuration&conf,DirScanner&dirS,FileProcessor&filePro) 
    :_dirScanner(dirS)
     /* ,_fileProcessor(filePro) */
     /* ,_conf(conf) */
{
}

//先调用DirScanner读取语料库目录下的所有xml文件名，存放于vector中
//再遍历一遍vector,将每篇xml文章按照指定格式解析成网页库的格式存放于_rss中
void PageLib::create(){
    _dirScanner(DIR_NAME);

    /* char*oldwd=getcwd(NULL,0); */
    /* cout<<"oldwd:"<<oldwd<<endl; */

    /* chdir(DIR_NAME); */
    /* cout<<"now cwd is:"<<getcwd(NULL,0)<<endl; */
    /* cout<<"_dirScanner has established a  filename vector successfully"<<endl; */
    vector<string>dir=_dirScanner.getFiles();
    for(auto &e:dir){
        read(e);
    }
    /* chdir(oldwd); */
    /* cout<<"after creat,now cwd is:"<<getcwd(NULL,0)<<endl; */
    /* cout<<"now cwd is:"<<getcwd(NULL,0)<<endl; */
}


//将解析后的文件持久化到磁盘上

void PageLib::store(){
    ofstream ofs1(RIPE_PATH);
    if(!ofs1){
        perror("open fail");
        std::cerr << "open " << RIPEPAGE << " fail!" << endl;
        return;
    }

    for(size_t idx = 0; idx != _rss.size(); ++idx){
        auto begin=ofs1.tellp();
        ofs1 << "<doc><docid>" << idx + 1
            << "</docid><url>" << _rss[idx]._url 
            << "</url><title>" << _rss[idx]._title 
            << "</title><content>" << _rss[idx]._content 
            << "</content></doc>";
        ofs1 << '\n';
        auto length=ofs1.tellp()-begin;
        _offsetLib[idx]=std::make_pair<uint64_t,uint64_t>(begin,length);
    }
    ofs1.close();

    //将偏移库写入磁盘文件OFFSET_PATHofstream ofs2(OFFSET_PATH);
     ofstream ofs2(OFFSET_PATH);
     if(!ofs2){
        std::cerr << "open " << OFFSET << " fail!" << endl;
        return;
    }

    for(size_t idx=0;idx!=_rss.size();++idx){
        ofs2<<idx<<" "<<_offsetLib[idx].first<<" "<<_offsetLib[idx].second<<"\n";
    }
    ofs2.close();
}

//读取rss文件filename并解析成网页库的形式
void PageLib::read(string &filename) {

    XMLDocument doc;
    /* std::cout<<"read file "<<filename<<endl; */
    //加载rss文件
    doc.LoadFile(filename.c_str());
    if(doc.ErrorID()){
        std::cerr << "loadFile"<<filename<<" fail" << endl;
        return;
    }

    size_t idx = 0;

    XMLElement *itemNode = doc.FirstChildElement("rss")->FirstChildElement("channel")->FirstChildElement("item");
    while(itemNode){
        string title, content;

        //获取文章标题
        auto it=itemNode->FirstChildElement("title");
        if(it) {
            title=it->GetText();
        } 

        //获取文章内容
        it= itemNode->FirstChildElement("description");
        if(it){
            content=it ->GetText();
        }else{
            it= itemNode->FirstChildElement("content");
            if(it){
                content=it->GetText();
            }
        }

        //去除文章标题和文章内容中的换行符'/n'
        int pos=0;
        int tcnt=0,ccnt=0;
        while((pos=title.find('\n',pos))!=-1)
        {
            title[pos]=' ';
            pos++;
            tcnt++;
        }

        pos=0;
        while((pos=content.find('\n',pos))!=-1)
        {
            content[pos]=' ';
            pos++;
            ccnt++;
        }


        /* cout<<"doc: "<<_rss.size()+1<<" space_num in title:"<<tcnt */
            /* <<" space_num in content:"<<ccnt<<endl; */
        string url=filename;

        if(title.empty()){//标题为空时，则选择文章内容的第一行作为标题
            std::istringstream is(content);
            getline(is,title);
        }

        //使用通用正则表达式过滤description和content
        std::regex reg("<[^>]+>");//通用正则表达式
        /* title = regex_replace(content, reg, ""); */
        content = regex_replace(content, reg, "");

        //将解析出的title\link\description\content存储到结构体RSSItem中
        RSSItem rssItem;

        rssItem._title = title;
        rssItem._content = title+content;
        /* rssItem._url=DIR_NAME; */
        /* rssItem._url+="/"; */
        rssItem._url=url;


        //将新生成的RSSItem存入vector中
        _rss.push_back(rssItem);
        ++idx;

        //获取下一个item
        itemNode = itemNode->NextSiblingElement("item");
    }
}


