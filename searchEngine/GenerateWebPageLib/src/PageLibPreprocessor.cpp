
#define LOGGER_LEVEL LL_WARN 
#include "../include/PageLibPreprocessor.h"

#include "../include/simhash/Simhasher.hpp"
#include<math.h>
using namespace simhash;
//根据配置信息读取ripepage.dat 和offset.dat的内容
//建立_pageList 
void PageLibPreprocessor:: readInfoFromFile()
{
    map<string,string>conf=_conf.getConfigMap();
    string PAGE_PATH=conf["RIPEPAGE"];
    
    //打开文件PATH_PATH, 每次读取一行，用来创建一个WebPage对象，直到读完
    std::ifstream infile;
    infile.open(PAGE_PATH);
    if(!infile.good()){
        std::cerr<<PAGE_PATH<<"open error"<<endl;
        return ;
    }
    string doc;
    while(getline(infile,doc)){
        //每次创建WebPage对象的时候，WebPage就会用doc中的内容初始化其数据成员
        _pageList.push_back(WebPage(doc,_conf,_wordCutter));
    }

}//readInfoFromFile


//根据_pageList中的信息进行去重
//每篇文档内容生成对应的simhash值
//将64bit的simhash值分成4段，分别建立4个unordered_set
//遍历一遍vector中的所有文章,
//文章-->simhash值-->分别zai4个unordered_set中查找是否有相同内容的
//若4部分中有任意一部分相同，则认为当前文章已存在相似版本的文章，从vector中删除当前文章
#if 1
void PageLibPreprocessor:: cutRedundantPage(){
    cout<<"begin cutRedundantPage"<<endl;

    Simhasher simhasher("../include/simhash/deps/dict/jieba.dict.utf8", 
                        "../include/simhash/deps/dict/hmm_model.utf8", 
                        "../include/simhash/deps/dict/idf.utf8", 
                        "../include/simhash/deps/dict/stop_words.utf8");
    unordered_set<uint16_t>s1,s2,s3,s4;
    cout<<"begin to travers _pageList"<<endl;;

    vector<WebPage>reduced;
    reduced.reserve(_pageList.size());

    //将不重复的WebPage对象暂存于reduced中，结束后，用reduced替代_pageList
   for(auto it=_pageList.begin();it!=_pageList.end();it++)
    {
        string doc=(*it).getDocContent();
        size_t topN = 20;//获取前topN的关键字
        uint64_t simValue = 0;//获取simhash值

        simhasher.make(doc, topN, simValue);

        //分割64bit的simValue成4部分的16bit 到arr
        vector<uint16_t>arr;
        arr.reserve(4);
        unsigned short *ptr=(unsigned short*)&simValue;
        for(int i=0;i<64/16;i++){
            arr.push_back(*ptr);
            ptr++;
        }
        /* cout<<"segmentation: "<<arr[0]<<" "<<arr[1]<<" "<<arr[2]<<" "<<arr[3]<<endl; */

        if(s1.count(arr[0])||s2.count(arr[1])||s3.count(arr[2]||s4.count(arr[3]))){
            //4部分中的任一部分已存在，则认为有该文章的相似文章已经存在
            /* cout<<"doc id:"<<it->getDocId()<<"has a similar ariticle already exist in _pageList"<<endl; */
        }else{
            reduced.push_back(std::move(*it));
            //保留当前it指向的元素
        }
        //更新4个set
        s1.insert(arr[0]);
        s2.insert(arr[1]);
        s3.insert(arr[2]);
        s4.insert(arr[3]);
    }
    cout<<"sizeof reduced:"<<reduced.size()<<endl;
    //更新_PageList
    cout<<"****begin renew PageList"<<endl;
    _pageList=std::move(reduced);
    cout<<"***endof  renew PageList"<<endl;


}//cutRedundantPage

#endif

#if 0
void PageLibPreprocessor:: cutRedundantPage(){
    cout<<"begin cutRedundantPage"<<endl;
        const int thread_hold=7;//distance<16 is considered the same

    Simhasher simhasher("../include/simhash/deps/dict/jieba.dict.utf8", 
                        "../include/simhash/deps/dict/hmm_model.utf8", 
                        "../include/simhash/deps/dict/idf.utf8", 
                        "../include/simhash/deps/dict/stop_words.utf8");
    vector<WebPage>reduced;
    reduced.reserve(_pageList.size());
    cout<<"begin to travers _pageList"<<endl;;

    //将不重复的WebPage对象暂存于reduced中，结束后，用reduced替代_pageList
    for(auto it=_pageList.begin();it!=_pageList.end();it++)
    {
        string doc=(*it).getDocContent();
        size_t topN = 20;//获取前topN的关键字
        uint64_t simValue = 0;//获取simhash值

        simhasher.make(doc, topN, simValue);

    // (Simhasher::isEqual(us1, us3,12)) << endl; 

        //traverse vector reduced, if no similar article exists,
        //push_back current article
        bool is_unique=true;
        for(auto ptr=reduced.begin();ptr!=reduced.end();ptr++){
            string doc=(*ptr).getDocContent();
            size_t topN = 20;//获取前topN的关键字
            uint64_t sim_comp = 0;//获取simhash值
            simhasher.make(doc, topN, sim_comp);
            if(Simhasher::isEqual(simValue,sim_comp,thread_hold)){
            cout<<"doc id:"<<it->getDocId()<<"has a similar ariticle already exist in _pageList"<<endl;
                is_unique=false;
                break;
            }
        }

        if(is_unique)
            reduced.push_back(*it);
    }
    cout<<"sizeof reduced:"<<reduced.size()<<endl;
    //更新_PageList
    cout<<"****begin renew PageList"<<endl;
    _pageList=std::move(reduced);
    cout<<"***endof  renew PageList"<<endl;


}//cutRedundantPage

#endif

#if 0
//创建倒排索引_invertIndexTable
void PageLibPreprocessor::bulidInvertIndexMap(){
    for(auto it=_pageList.begin();it!=_pageList.end();++it){
        /* cout<<"in 1 loop"<<endl; */
        unordered_map<string,int> TF=it->getWordsMap();
        int id=it->getDocId();

        map<string,double> w;
        for(auto p=TF.begin();p!=TF.end();++p){
            /* cout<<"in 2 loop"<<endl; */
            int cnt=0;
            for(auto a=_pageList.begin();a!=_pageList.end();++a){
                /* cout<<"in 3 loop"<<endl; */
                if(a->getWordsMap().find(p->first)!=a->getWordsMap().end()){
                    cnt++;
                }
            }
            double b=log(_pageList.size()/(cnt+1))/log(2)*p->second;
            w.insert(make_pair(p->first,b));
        }

        double temp=0;
        for(auto it=w.begin();it!=w.end();++it){
            /* cout<<"in 4 loop"<<endl; */
            temp+=it->second*it->second;
        }
        temp=sqrt(temp);

        for(auto p=w.begin();p!=w.end();++p){
            /* cout<<"in small loop"<<endl; */
            double q=p->second/temp;
            pair<int,double> a(id,q);
            _invertIndexLib[p->first].push_back(a);
        }
    }

}//buildInverIndexMap

#endif 

#if 1

void PageLibPreprocessor::bulidInvertIndexMap(){

    unordered_map<string,int> DFmap;//存放所有网页的单词，DF
    //遍历一遍_pageList,生成DFmap
    for(auto it=_pageList.begin();it!=_pageList.end();it++)
    {
        unordered_map <string,int>&TFmap=(*it).getWordsMap();

        //遍历当前网页的词典，将每个词都加入到DFmap中
        for(auto it1=TFmap.begin();it1!=TFmap.end();it1++){
            DFmap[it1->first]++;
        }
    }

    int N=_pageList.size();//网页总数
    //再次遍历_pageList,
    for(auto it=_pageList.begin();it!=_pageList.end();it++)
    {
        //对每篇webPage对象，计算它每个单词的IDF, W W'
        //然后将该篇文章每个单词的IDF,文章id， w' 存入_invertIndexTable中
        
        double weight_sum=0;//当前文章的权重平方和
        //第一遍遍历遍历当前文章的TFmap, 求出weight_sum
        unordered_map <string,int>&TFmap=(*it).getWordsMap();
        vector<pair<string,double>>words;//用于暂存TFmap中的单词和其w,便于后续遍历
        words.reserve(TFmap.size());
        for(auto it1=TFmap.begin();it1!=TFmap.end();it1++){
            string word=it1->first;
            double tf=it1->second;
            double df=DFmap[word];
            double idf=log(N/df+1)/log(2);
            double w=tf*idf;
            weight_sum+=w*w;
           words.push_back(pair<string,double>(word,w));
        }

        double para_w=sqrt(weight_sum);
        /* cout<<"sizeof vector words :"<<words.size()<<endl; */
        //第二遍遍历，遍历vector，计算其中每个单词的w'  并插入到invertIndexTable中
        for(auto &e:words){
            string word=e.first;
            double w_=e.second;
            double final_w=w_/para_w;
            int id=it->getDocId();
            pair<int ,double>value_pair(id,final_w);
            _invertIndexLib[word].push_back(value_pair);
        }//for
    }//for

}

#endif

//将去重后的网页库，偏移库和倒排索引库存到磁盘上
void PageLibPreprocessor:: storeOnDisk()
{

    map<string,string>conf=_conf.getConfigMap();
    string PAGE_PATH=conf["NEWRIPEPAGE"];
    string OFFSET_PATH=conf["NEWOFFSET"];
    string INVERT_PATH=conf["INVERT"];

    //存储去重后新的网页库
    ofstream ofs1(PAGE_PATH);
    if(!ofs1){
        perror("open fail");
        std::cerr << "open " << PAGE_PATH << " fail!" << endl;
        return;
    }

    for(auto it=_pageList.begin();it!=_pageList.end();it++)
    {
       int docid=it->getDocId();
        auto begin=ofs1.tellp();
        ofs1 << "<doc><docid>" << docid
            << "</docid><url>" << it->getDocUrl() 
            << "</url><title>" << it->getDocTitle() 
            << "</title><content>" << it->getDocContent() 
            << "</content></doc>";
        ofs1 << '\n';
        auto len_=ofs1.tellp()-begin;
        _offsetLib[docid]=std::make_pair<uint64_t,uint64_t>(begin,len_);
    }
    ofs1.close();

    //将偏移库写入磁盘文件OFFSET_PATHofstream ofs2(OFFSET_PATH);
    ofstream ofs2(OFFSET_PATH);
    if(!ofs2){
        std::cerr << "open " << OFFSET_PATH << " fail!" << endl;
        return;
    }

    for(auto &e:_offsetLib){
        ofs2<<e.first<<" "<<e.second.first<<" "<<e.second.second<<"\n";
    }
    ofs2.close();


    //存储倒排索引库
    ofstream ofs3(INVERT_PATH);
    if(!ofs3){
        std::cerr << "open " << OFFSET_PATH << " fail!" << endl;
        return;
    }

    cout<<"sizeof inverttable:"<<_invertIndexLib.size()<<endl;
    for(auto Mit=_invertIndexLib.begin();Mit!=_invertIndexLib.end();Mit++){
        const string&word=Mit->first;
        vector<std::pair<int,double>>&vec=Mit->second;
        ofs3<<word;//单词
        //单词出现的额文章id和权重
        for(auto &e:vec){
            ofs3<<" "<<e.first<<" "<<e.second;
        }
        ofs3<<endl;
    }


    ofs3.close();
}//storeOnDisk
