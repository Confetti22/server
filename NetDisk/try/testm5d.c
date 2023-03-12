#include<func.h>
#include"md5.h"

int main(int argc, char* argv[]){
 ARGS_CHECK(argc,2);
    
    char parabuf[1024]={0};
    strcpy(parabuf,(const char*)argv[1]);
    char md5_str[MD5_STR_LEN+1]={0};
    char sendbuf[4096]={0};
    //先查找本地是否存在 filename
    int fd=open(parabuf,O_RDONLY);
    if(fd==-1&&errno==ENOENT){
        //filename 本地不存在
        printf("file:%s does not exit\n",parabuf);
    }else if(fd==-1){
        ERROR_CHECK(fd,-1,"open");
    }
    else{
        //获取本地待上传的文件的md5值
        int ret=Compute_file_md5(parabuf,md5_str);
        ERROR_CHECK(ret,-1,"Computer_file_md5");
printf("md5:\n%s\n",md5_str);
        //给server 发送 puts  md5


    }
 return 0;
}
