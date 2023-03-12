#include"../include/ProtocolParser.h"
TaskType ProtocolParser::operator()(const string&msg,const TcpConnectionPtr &con ){
    string id=msg.substr(0,1);
    string sought=msg.substr(2);
    cout<<"in ProtocolParser , id="<<id<<"  sought="<<sought<<endl;
    if(id=="1"){
        return nullptr;

    }else if(id=="2"&&sought.size()!=0){
        WebPageSearcher newsearcher(sought,con,_webPageQuery,_cacheManager);
        //void doQuery(WebPageQuery类型的指针)---> void doQuery()

        TaskType task= std::bind (&WebPageSearcher::doQuery,newsearcher);
        cout<<"in ProtocolParser:: generated a task"<<endl;
        return task;

    }else{
        std::cerr<<"wrong msg foramt, parser cannot parse!"<<std::endl;
        return nullptr;
    }
}
