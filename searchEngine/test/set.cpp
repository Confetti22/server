#include<iostream>
#include<string>
#include<set>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
using std::cout;
using std::endl;
using std::set;
using std::string;
void test0(){
    set<string>set1;
    set1.insert("apple");
    set1.insert("zara");
    set1.insert("zebra");
    set1.insert("elephant");
    set1.insert("塔塔");
    set1.insert("一");
    set1.insert("十一");
    set1.insert("七十七");
    set1.insert("啊");
    set1.insert("凌乱");

    cout<<"sizeof set:"<<set1.size()<<endl;
    set<string>::iterator it=set1.find("啊");
    cout<<"find result:"<<*it<<endl;
    for(auto &e:set1){
        cout<<e<<" ";
    }cout<<endl;
}

void test1(){
    std::vector<int> v1{1,2,3,4,5,6,7,8};
    std::vector<int> v2{        5,6,7,  9,10};
    std::vector<int> v3{    3,  5,  7,  9,10};
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    std::sort(v3.begin(), v3.end());
 
    std::vector<int> v_intersection;
 
    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(v_intersection));
    for(int n : v_intersection)
        std::cout << n << ' ';
    std::cout<<std::endl;

    std::vector<int>v_result;
    std::set_intersection(v_intersection.begin(), v_intersection.end(),
                          v3.begin(), v3.end(),
                          std::inserter(v_result,v_result.begin()));
    for(int n : v_result)
        std::cout << n << ' ';
    std::cout<<std::endl;
}



int main(){

    test1();
}
