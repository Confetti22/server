#include <iostream>
#include<iomanip>
#include <fstream>

//this define can avoid some logs which you don't need to care about.
#define LOGGER_LEVEL LL_WARN 

#include "../include/simhash/Simhasher.hpp"
using namespace simhash;

int main(int argc, char** argv){
    Simhasher simhasher("../include/simhash/deps/dict/jieba.dict.utf8", 
                        "../include/simhash/deps/dict/hmm_model.utf8", 
                        "../include/simhash/deps/dict/idf.utf8", 
                        "../include/simhash/deps/dict/stop_words.utf8");
    string s1("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    size_t topN = 10;//获取前topN的关键字
    uint64_t us1 = 0;//获取simhash值
    uint64_t us2 = 0;//获取simhash值
    uint64_t us3 = 0;//获取simhash值
    vector<pair<string ,double> > res;//保存前topN的关键词与权重

    //extract 字符串s 的前topN个词频最高的关键词，并将单词与对应的权重保存到res中
    /* simhasher.extract(s1, res, topN); */

    //根据字符串的前topN的关键字生成对应的simhash值？
    simhasher.make(s1, topN, us1);
    cout<< "文本：\"" << s1 << "\"" << endl;
    cout << "关键词序列是: " << res << endl;
    cout<< ">>>s1 simhash值是: " << setiosflags(ios_base::hex)<<us1<<endl;

    string s2("我是红翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    /* simhasher.extract(s2, res, topN); */
    simhasher.make(s2, topN, us2);
    cout<< "文本：\"" << s2 << "\"" << endl;
    cout << "关键词序列是: " << res << endl;
    cout<< ">>>s2 simhash值是: " << setiosflags(ios_base::hex)<<us2<<endl;

    string s3("我们是技工学院修拖拉机的。很快，我们都会有光明的未来，过上小康生活");
    /* simhasher.extract(s3, res, topN); */
    simhasher.make(s3, topN, us3);
    cout<< "文本：\"" << s3 << "\"" << endl;
    cout << "关键词序列是: " << res << endl;
    cout<< ">>s3 simhash值是: " << setiosflags(ios_base::hex)<<us3<<endl;

    cout<< "s1" << "和" << "s2" << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为20，则isEqual结果为：" 
        << (Simhasher::isEqual(us1, us2,5)) << endl; 

    cout<< "s1" << "和" << "s3" << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为20，则isEqual结果为：" 
        << (Simhasher::isEqual(us1, us3,12)) << endl; 

    cout<<">>>>>>>>>test segmentation a uint64_t "<<endl;
vector<uint16_t>arr1,arr2;

unsigned short *ptr1,*ptr2;
ptr1=(unsigned short *)&us1;
ptr2=(unsigned short *)&us2;

for(int i=0;i<4;i++){
  arr1.push_back(*ptr1);
  arr2.push_back(*ptr2);
  ptr1++;
  ptr2++;
}

    printf("us1:%lx\n",us1);
    printf("us2:%lx\n",us2);
cout<<"us1:"<<endl;
for(auto &e:arr1){
    printf("size:%lu  ",sizeof(e));
    printf("%hx\n",e);
}cout<<endl;

cout<<"us2:"<<endl;
for(auto &e:arr2){
    printf("size:%lu  ",sizeof(e));
    printf("%hx\n",e);
}cout<<endl;

    const char * bin1 = "100010110110";
    const char * bin2 = "110001110011";
    uint64_t u1, u2;
    u1 = Simhasher::binaryStringToUint64(bin1); 
    u2 = Simhasher::binaryStringToUint64(bin2); 
    
    cout<< bin1 << "和" << bin2 << " simhash值的相等判断如下："<<endl;
    cout<< "海明距离阈值默认设置为3，则isEqual结果为：" 
        << (Simhasher::isEqual(u1, u2)) << endl; 

    cout<< "海明距离阈值默认设置为5，则isEqual结果为："
        << (Simhasher::isEqual(u1, u2, 5)) << endl; 
    return EXIT_SUCCESS;
}
