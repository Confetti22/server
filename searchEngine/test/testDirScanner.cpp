#include"../include/DirScanner.h"
#include<iostream>
using namespace std;
int main(int argc,char*argv[]){
    ARGS_CHECK(argc, 2);
    DirScanner d;
    d(argv[1]);
    vector<string>dir=d.getFiles();
    for(auto &e:dir){
        cout<<e<<endl;
    }

    return 0;
}

