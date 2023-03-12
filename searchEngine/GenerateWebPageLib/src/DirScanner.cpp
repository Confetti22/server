#include"../include/DirScanner.h"
#include<stdio.h>
#include<func.h>



DirScanner::DirScanner()
{
    _files.reserve(128);
}

vector<string>& DirScanner::getFiles()
{
    return _files;
}

bool isdir(struct dirent* pdirent){
    return DT_DIR==pdirent->d_type&&strcmp(pdirent->d_name, ".")&&strcmp(pdirent->d_name, "..");
}


void DirScanner::operator()(string dir)
{
    traverse(dir);
}

void DirScanner::traverse(string dirname)
{
    //打开目录流dirname
    DIR*pdir=opendir(dirname.c_str());  
    ERROR_CHECK(pdir,NULL,"opendir");
    struct dirent* pdirent=readdir(pdir);

    //遍历dirname的目录项，若某一个目录项为目录，则递归进行上述过程
    while(true){
        if(pdirent==NULL) break;
        //如果当前目录项是一个目录（不包括. ..）
        //则递归地遍历该目录
        if(isdir(pdirent)){ 
            traverse(dirname+'/'+pdirent->d_name);
            pdirent=readdir(pdir);
        }else{//当前目录项是一个普通文件

            if(strcmp(pdirent->d_name,"..")&&strcmp(pdirent->d_name,".")){//当前目录项不是目录也不是. ..
                _files.push_back(dirname+'/'+pdirent->d_name);
            }
            pdirent=readdir(pdir);
        }
    }//while
    closedir(pdir);
}

