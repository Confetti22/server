#ifndef __parser_H__
#define __parser_H__

#define OPMAXLEN 10
#define PARAMAXLEN 128
enum TASKTYPE{
    TASK_REGISTER,
    TASK_LOGIN,
    TASK_COMMAND
};

enum OPTION_CODE{
    REGISTER,
    LOGIN,
    PUTS,
    GETS,
    CD,
    LS,
    RM,
    MKDIR,
    PWD,
    EXIT
};

enum CLNTTYPE{
    CLNTPUTS,
    CLNTGETS,
    CLNTEXIT,
    CLNTSHORT //短命令
};

//解析指令str,  返回指令类型， 并将指令参数存放到para中
int  getOPTION_CODE(char*str,int *op_code,char*op_para);

int getTASKTYPE(char * str,char*parabuf);

int getClientCommandType(char *str,char*parabuf);
int getClientCommandType(char *str,char*parabuf);
int getClientCommandType(char *str,char*parabuf);
int getClientCommandType(char *str,char*parabuf);
int getClientCommandType(char *str,char*parabuf);
#endif
