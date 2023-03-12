#include<iostream>
#include<unordered_map>
#include<fstream>
#include<sstream>
#include<string>
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;


class UDictionary
{
public:
    UDictionary(){
        cout<<"Dictonary()"<<endl;
    }
    ~UDictionary(){
        cout<<"~UDictionary()"<<endl;
    }
    void read(const std::string &filename);
    void store(const std::string &filename);
    string cleanup_str(const string &word);                             
private:
	unordered_map<string,size_t> _dict;
};
void UDictionary::read(const std::string &filename){
    //创建一个文件流对象infile, 并将他绑定到给定的文件filename
    std::ifstream infile;
    infile.open(filename);
    if(!infile.good()){
        std::cerr<<filename<<"open error"<<endl;
        return ;
    }
/**********创建_dict***********/   
    //不断地从infile中读出一行数据
   string line;
   while(getline(infile,line)){
       //从一行数据line中拆解出一个个单词
       std::istringstream is(line);
       string word;
       while(is>>word){
           word=cleanup_str(word);
           auto it=_dict.find(word);
           if(it!=_dict.end())
               //_dict中已经存在该word
               it->second++;//词频++
           else
               _dict[word]=1;//该单词第一次出现
       }//while(is>>word)
   }


    //关闭文件流
    infile.close();

}
void UDictionary::store(const std::string &filename){

   std::ofstream outfile;
   outfile.open(filename);
   if(!outfile.good()){
       std::cerr<<filename<<"open error"<<endl;
   }

   for(const auto&e:_dict){
       outfile<<e.first<<" "<<e.second<<endl;
   }
   outfile.close();
}
 // not covered in the book -- cleanup_str removes
 // punctuation and converts all text to lowercase so that
 // the queries operate in a case insensitive manner
 string UDictionary::cleanup_str(const string &word)                             
 {
     string ret;
     for (string::const_iterator it = word.begin(); 
                 it != word.end(); ++it) {
         if (isalpha(*it))
             ret += tolower(*it);
     }
     return ret;
 }

int main(int argc, char* argv[]){
    if(argc!=3){
        std::cerr<<"pleas input two arg!"<<endl;
    }
    UDictionary d1;
    clock_t start,end;
    start=clock();
    d1.read(argv[1]);
    d1.store(argv[2]);
    end=clock();
    cout<<"time = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<endl;  //输出时间（单位：ｓ）
}

