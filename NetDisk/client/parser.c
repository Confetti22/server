#include<func.h>
#include"parser.h"

int  getOPTION_CODE(char*str ,int*op_code,char*op_para){
    char op[OPMAXLEN]={0};
    sscanf(str,"%s %s",op, op_para);
    if(strcmp(op,"REGISTER")==0)
        *op_code= 0;
    else if(strcmp(op,"LOGIN")==0)
        *op_code= 1;
    else if(strcmp(op,"puts")==0)
        *op_code= 2;
    else if(strcmp(op,"gets")==0)
        *op_code= 3;
    else if(strcmp(op,"cd")==0)
        *op_code= 4;
    else if(strcmp(op,"ls")==0)
        *op_code= 5;
    else if(strcmp(op,"rm")==0)
        *op_code= 6;
    else if(strcmp(op,"mkdir")==0)
        *op_code= 7;
    else if(strcmp(op,"pwd")==0)
        *op_code= 8;
    else if(strcmp(op,"exit")==0||strcmp(op,"EXIT")==0)
        *op_code= 9;
    else *op_code= -1;//非法命令

return 0;

}

 
//根据op_code返回任务类型 （-1 非法命令   0 Register    1 Login  2 Command）
int getTASKTYPE(char* str, char*parabuf){
    int taskType;
    int op_code=0;
    getOPTION_CODE(str,&op_code,parabuf);
    switch(op_code){
    case -1:taskType=-1;break;
    case REGISTER: taskType=TASK_REGISTER;break;
    case LOGIN: taskType=TASK_LOGIN;break;
    default: taskType=TASK_COMMAND;break;
    }
    return taskType;
}


//返回客户端需要解析的命令类型
#if 1 
int getClientCommandType(char *str,char*parabuf){
    int clntType;
    int op_code=0;
    getOPTION_CODE(str,&op_code,parabuf);
    switch(op_code){
    case -1:clntType=-1;break;//非法命令=-1
    case PUTS: clntType=CLNTPUTS;break;//puts=1 上传文件
    case GETS: clntType=CLNTGETS;break;//gets=2  下载文件
    case EXIT: clntType=CLNTEXIT;break;//puts=3 上传文件
    default: clntType=CLNTSHORT;break;//exit=4 上传文件
    }
    return clntType;
}
#endif
