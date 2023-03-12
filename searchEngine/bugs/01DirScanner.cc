#include"../include/DirScanner.h"
DirScanner::DirScanner()
{
    _files.reserve(128);
}
vector<string>& DirScanner::getFiles()
{
    return _files;
}
void DirScanner::traverse(string dir)
{
    //opendir
    DIR *pdir = opendir(dir.c_str());
    ERROR_CHECK(pdir, NULL, "opendir");

    char*oldwd=getcwd(NULL,0);

    //change current working dirctory
    chdir(dir.c_str()); 

    // 读写目录项
    struct dirent* pdirent;
    while ((pdirent = readdir(pdir)) != NULL) {
        if(pdirent->d_name[0]!='.')//.  .. 不会计入
        _files.push_back(dir+'/'+pdirent->d_name);
    }

    chdir(oldwd); 
    closedir(pdir);

}



void DirScanner::operator()(string dir)
{
    traverse(dir);
}


