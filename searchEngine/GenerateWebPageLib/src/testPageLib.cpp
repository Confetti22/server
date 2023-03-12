#include"../include/PageLib.h"
#include<iostream>


void test1(){
    PageLib mylib;
    std::cout<<"begin mylib.creat()"<<std::endl;
    mylib.create();
    std::cout<<"finish mylib.creat()"<<std::endl;
    mylib.store();
    std::cout<<"finish mylib.store()"<<std::endl;

}


void test2(string filename){

    PageLib mylib;
    std::cout<<"begin mylib.creat()"<<std::endl;
    mylib.read(filename);
    std::cout<<"finish mylib.read()"<<std::endl;
    mylib.store();
    std::cout<<"finish mylib.store()"<<std::endl;


}
int main(int argc, char*argv[]){
    /* test2(argv[1]); */
    test1();


    return 0;

}
