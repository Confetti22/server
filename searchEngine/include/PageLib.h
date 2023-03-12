/**
 * Project SearchEngine
 */

#ifndef _PAGELIB_H
#define _PAGELIB_H
#include"DirScanner.h"
#include"Configuration.h"
class FileProcessor;

struct RSSItem
{
    string _url;//文章的路径
    string _title;//文章的标题，若title为空，则提取文章内容的第一行为标题
    string _content;//文章的内容
};


class PageLib {
public: 
    PageLib();
    PageLib(Configuration&conf,DirScanner&dirScanner,FileProcessor&files);
    void create();
    void store();

    //具体解析一遍xml文档的函数
    void read(string &filename) ;
private: 
    DirScanner _dirScanner;
    /* FileProcessor &_fileProcessor; */
    /* Configuration &_conf; */
    vector<RSSItem> _rss;
    map<uint32_t, pair<uint64_t,uint64_t>>_offsetLib;
};

#endif //_PAGELIB_H
