
#include "../include/WebPageQuery.h"
#include<algorithm>
#include<iostream>
#include<sstream>
#include<math.h>

WebPageQuery::WebPageQuery(SplitTool&cutter,Configuration& conf)
    :_conf(conf),
    _wordCutTool(cutter)
{
    cout<<"WebPageQuery(SplitTool&cutter,Configuration& conf=*pConf)"<<endl;
    readInfoFromFile();
    cout<<"After WebPageQuery::readInfoFromFile()"<<endl;
}


//<文章id, 该文章特征向量与基准向量的余弦值>
//vector<pair<int ,double>>cosineValues;
//比较函数
bool mycompare(pair<int ,double>lhs,pair<int ,double> &rhs)
{
    return lhs.second>=rhs.second;
}


//vector<pair<int ,vector<double>>>vecList;
void printVecList(vector<pair<int ,vector<double>>>&vecList){
    for(auto idx:vecList){
        int id=idx.first;
        vector<double>vec=idx.second;
        cout<<"id:"<<id<<"  特征向量：";
            for(auto e:vec){
                cout<<e<<" ";
            }cout<<endl;
    }
}

void printvec(vector<double>&base){
    for(auto e:base){
        cout<<e<<" ";
    }cout<<endl;
}

void printCosValues(vector<pair<int ,double>>&csv)
{

    for(auto e:csv){
        cout<<e.first<<" "<<e.second<<endl;
    }cout<<endl;
}

//返回vec向量与基准向量base之间的余弦值
double getcos(vector<double>&vec,vector<double>&base){
    double vecDOTbase=0;
    double vec_len=0,base_len=0;
    for(size_t idx=0;idx<vec.size();idx++){
        vecDOTbase+=vec[idx]*base[idx];
        vec_len+=vec[idx]*vec[idx];
        base_len+=base[idx]*base[idx];
    }
    vec_len=sqrt(vec_len);
    base_len=sqrt(base_len);

    if (vec_len&&base_len){
        return vecDOTbase/(vec_len*base_len);
    }else{
        return -1;
    }
}

//返回vec向量与基准向量base之间的余弦值
//double getcos(vector<double>&vec,vector<double>&base){
//带查询关键词们-->基准向量
vector<double>WebPageQuery::getQueryWordsWeightVec(vector<string>&queryWords){
    vector<double>base;//基准向量

    //计算权重
    double weight_sum=0;
    base.reserve(queryWords.size());
    for(size_t idx=0;idx<queryWords.size();idx++){
        string key=queryWords[idx];
        double tf=1;
        double N=_offsetlib.size()+1;
        double df =(_invertIndexLib[key]).size()+1;
        double idf=log(N/df+1)/log(2);
        double w=tf*idf;
        base.push_back(w);
        weight_sum+=w*w;
    }

    //权重归一化
    double para_w=sqrt(weight_sum);
    for(size_t idx=0;idx<base.size();idx++){
        base[idx]=base[idx]/para_w;
    }
    return base;
}




void WebPageQuery::readInfoFromFile()
{
    map<string,string>conf=_conf.getConfigMap();
    string OFFSET_PATH=conf["NEWOFFSET"];
    string INVERT_PATH=conf["INVERT"];

    //①将倒排库的信息读入内存
    std:: ifstream if_invert;
    if_invert.open(INVERT_PATH);
    if(!if_invert.good()){
        std::cerr<<INVERT_PATH<<"open error"<<endl;
        return ;
    }

    string line;
    while(getline(if_invert,line)){
        //unordered_map<string,vector<pair<int, double>>> _invertIndexLib;
        std::istringstream is(line);
        string word;
        int no;
        double w;
        is>>word;
        while(is>>no>>w){
            _invertIndexLib[word].push_back(pair<int, double>(no,w));
        }
    }
    if_invert.close();
    //②将偏移库的信息读入内存
    std:: ifstream if_offset;
    if_offset.open(OFFSET_PATH);
    if(!if_offset.good()){
        std::cerr<<OFFSET_PATH<<"open error"<<endl;
        return ;
    }
    line="";
    while(getline(if_offset, line)){
        int no;
        int begin=0;
        int end=0;
        std::istringstream is(line);
        is>>no>>begin>>end;
        _offsetlib[no]=pair<int,int>(begin, end);
    }
    if_offset.close();
}




string WebPageQuery::doQuery(string sought) {
    string result;
    //①分词：string->vector<string>　
    vector<string>keys=_wordCutTool.cut(sought);

    cout<<"分词结果："<<endl;
    for(auto e:keys){
        cout<<e<<",";
    }cout<<endl;

    //用户输入的sought中不能分出关键词，则查询无结果
    if(keys.size()==0){
        return result;
    }

    //articleIdlist中按照关键词的顺序，存放关键词出现的文章id集合
    vector<set<int>>articleIdList; 

    //②对于每个单词，将倒排表中查到的信息建立一个从文章编号no-->文章权重w的映射
    vector<unordered_map<int ,double>> weights;

    bool isfind=true;
    for(size_t idx=0;idx<keys.size();idx++){
        //对于关键词keys[idx] 在倒排表中查找他的数据
        string keyword=keys[idx];
        auto it=_invertIndexLib.find(keyword);
        if(it!=_invertIndexLib.end()){
            //如果倒排表中由关键词keyword的倒排信息，则将倒排信息建立成一个（id-->weight）的映射后后再存入vector weights中
            vector<pair<int, double>>& idWeights=it->second;
            unordered_map<int ,double> newdata;
            set<int>ids;
            for(auto & elem:idWeights){
                newdata[elem.first]=elem.second;
                ids.insert(elem.first);
            }
            weights.push_back(std::move(newdata));  //移动语义，提升效率
            articleIdList.push_back(std::move(ids));
        }else{
            //倒排表中没有关键词keyword的倒排信息，则终止查找，查询结果为空
            isfind=false;
            break;
        }

    }


    if(isfind){

        //求articlesIdList的交集， 如果交集为空，则查找结束
        set<int>interset;
        set<int>lastInterset;

        //先求List中前两个集合的交集
        bool hasinterset=true;

        if(articleIdList.size()>=2){
            set_intersection(articleIdList[0].begin(),articleIdList[0].end(),
                             articleIdList[1].begin(),articleIdList[1].end(),
                             std::inserter(interset,interset.begin()));

            if(interset.size()==0)
                hasinterset=false;

            lastInterset=interset;

        }else{
            //只有一个关键词，则无需求交集， 
            //交集就是articleIdList[0];
            lastInterset=articleIdList[0];
        }

        //对List中的剩余集合依次求交集
        for(size_t idx=2;idx<articleIdList.size()&&hasinterset;idx++){
            set<int>newInterset;
            set_intersection(lastInterset.begin(),lastInterset.end(),
                             articleIdList[idx].begin(),articleIdList[idx].end(),
                             std::inserter(newInterset,newInterset.begin()));

            hasinterset=(newInterset.size()!=0);
            lastInterset=std::move(newInterset);
        }
        //最终求得所有集合的交集interset(存放了所有文章的id)
        interset=lastInterset;

        cout<<"sizeof aiticleIdList(向量的维度,关键词个数)"<<articleIdList.size()<<endl;
        cout<<"sizeof interset（有所有关键词的文章个数，向量个数）"<<interset.size()<<endl;


        //vector<pair<int ,vector<double>>>构建每篇文章的特征向量
        vector<pair<int ,vector<double>>>vecList;
        vecList.reserve(interset.size());
        for(auto it=interset.begin();it!=interset.end();it++){
            int articleId=*it;
            vector<double>vec;
            //vector<unordered_map<int ,double>> weights;
            for(auto it2=weights.begin();it2!=weights.end();it2++){
                vec.push_back( (*it2)[articleId] );
            }
            vecList.push_back(pair<int ,vector<double>>(articleId,std::move(vec)));
        }
        /* cout<<"....特征向量：........"<<endl; */
        /* printVecList(vecList); */

        //带查询关键词们-->基准向量
        vector<double>base=getQueryWordsWeightVec(keys);
        /* cout<<"....基准向量：........"<<endl; */
        /* printvec(base); */
        

        //<文章id, 该文章特征向量与基准向量的余弦值>
        vector<pair<int ,double>>cosineValues;
        cosineValues.reserve(vecList.size());
        if(articleIdList.size()>=2){
            //2个及以上关键词
        for(auto &e:vecList){
            cosineValues.push_back(pair<int ,double>(e.first,getcos(e.second,base)));
        }

        }else{
            //只有一个关键词
        for(auto &e:vecList){
            cosineValues.push_back(pair<int ,double>(e.first,e.second[0]));
        }
            
        }

        /* cout<<"cosValuse: Before sort:"<<endl; */
        /* printCosValues(cosineValues); */
        //根据与基准向量之间余弦值的大小（由大到小）对文章id排序
        sort(cosineValues.begin(),cosineValues.end(),mycompare);

        /* cout<<"cosValuse: After sort:"<<endl; */
        /* printCosValues(cosineValues); */
        //去磁盘中找出前3篇文章到内存中的WebPage对象
        vector<WebPage>targetWebs;

        //打开网页库文件
        std::ifstream infile;
        map<string ,string>conf=_conf.getConfigMap();
        string NEWRIPEPAGE=conf["NEWRIPEPAGE"];

        infile.open(NEWRIPEPAGE);
        if(!infile.good()){
            std::cerr<<NEWRIPEPAGE<<"open error"<<endl;
            return nullptr ;
        }

        for(size_t i=0;i<3&&i<cosineValues.size();i++){
            int id=cosineValues[i].first;
            int begin=_offsetlib[id].first;
            int length=_offsetlib[id].second;
            char *pbuf=new char[length+1]();
            string doc;

            infile.seekg(begin);//移动文件指针到begin位置
            infile.read(reinterpret_cast<char*>(pbuf),length);//从begin位置读取lenght个字节到pbuf所指空间
            doc=pbuf;
            delete[]pbuf;
            targetWebs.push_back(WebPage(doc,_conf,_wordCutTool));
        }

        //根据需求，摘取出这3篇文章的关键信息打包到一个string对象中返回给客户端
        for(size_t i=0;i<targetWebs.size();i++){
            std::ostringstream os;
            os<<"^^^^^^^The "<<i+1<<"article^^^^^^^^\n";
            os<<"article_id=";
            os<<targetWebs[i].getDocId();
            os<<"\n";
            os<<"title=";
            os<<targetWebs[i].getDocTitle();
            os<<"\n\n";
            result+=os.str();
        }


    }else{
        //倒排表中不能全部找到待查询关键词的信息，查询结果为空
    }
    return result;
}
