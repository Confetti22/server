#include<iostream>
#include"../include/head.h"
using namespace std;

int main(){
    std::ifstream infile;
    infile.open("../data/newripepage.dat");
    if(!infile.good()){
        std::cerr<<"open error"<<endl;
    }

    int id=1;
    int begin=0;
    int length=3249;

    string doc;
    char buf[40960]={0};
    char*pbuf=new char[length+1];
    doc.reserve(length+10);

    infile.seekg(begin);
    infile.read(reinterpret_cast<char*>(&doc[0]),length);

    /* infile.seekg(0,ios::beg); */
    infile.seekg(begin);

    infile.read(reinterpret_cast<char*>(pbuf),length);
    cout<<"doc:"<<endl;
    cout<<doc<<endl;

    cout<<"pbuf"<<endl;
    cout<<pbuf<<endl;




    return 0;
}

