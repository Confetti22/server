#include<func.h>
#include"mysql.h"

int main(int argc,char*argv[]){
    ARGS_CHECK(argc,2);
    char sendbuf[4096]={0};
    printf("query:%s\n",argv[1]);
    int ret=mysql_file(argv[1],NULL,NULL,sendbuf,NULL,NULL,NULL);
    ERROR_CHECK(ret,-1,"mysql_file");
    printf("after query, sendbuf=%s\n",sendbuf);
    strcpy(sendbuf,"\n");
return 0;
}
