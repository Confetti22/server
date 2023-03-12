#include<func.h>
#include"parser.h"

int main(){
     char str1[128]="cd /user/bin/include";
     char str2[128]="exit";
    
    char para1[PARAMAXLEN]={0};
    char para2[PARAMAXLEN]={0};
   
    int op1;
    getOPTION_CODE(str1,&op1,para1);
    int op2;
    getOPTION_CODE(str2,&op2,para2);

    printf("op_type:%d, para:%s\n",op1,para1);
    printf("op_type:%d, para:%s\n",op2,para2);

}
