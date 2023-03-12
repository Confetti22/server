#include<func.h>
#include<ctype.h>

int GeneTkcode(char*pathname,char*strToken,int (*ptr));
enum TKCODE{
    DOT,      //"."
    DIR_NAME, //"dir_name"
    DOT_DOT,  // ".."
    SLASH,   //"/"
    ILLEGAL
};

int main(int argc,char*argv[]){
    ARGS_CHECK(argc,2);
    char pathname[1024]={0};
    strcpy(pathname,argv[1]);
    printf("pathname:%s\n",pathname);
    int ptr=0;
    char strToken[1024]={0};
    char correct=1;
    for(;pathname[ptr]!='\0'&&correct;){
        switch(GeneTkcode(pathname,strToken,&ptr)){
        case DOT:
            printf("DOT:");
            printf("strToken:%s\n",strToken);
            ;break; //do nothing
        case DIR_NAME:
            printf("DIR_NAME:");
            printf("strToken:%s\n",strToken);
            break;
        case DOT_DOT:
           printf("DOT_DOT");
            printf("strToken:%s\n",strToken);
            break;
        case SLASH://将当前目录设置为0
            printf("SLASH");
            printf("strToken:%s\n",strToken);
            break;
        default://解析处理非法的目录结构
            printf("cannot decipher the pathname,abort\n");
            printf("strToken:%s\n",strToken);
            correct=0;
        }
        memset(strToken,0,1024);

    }
}

int GeneTkcode(char*pathname,char*strToken,int *ptr){
    //(*ptr)指向buf中下一个要读取的字符
    //i指向strToken中下一个放入字符的位置
    int i = 0;
    strToken[0] = '\0';   /*置strToken为空串*/
    char ch = pathname[(*ptr)++];//从buf中读取一个字符到ch
    if(isalpha(ch)||isdigit(ch)||ch=='_'){
        strToken[i++] = ch; //ch链接到strToken后
        ch = pathname[(*ptr)++];//从buf中读取一个字符到ch
        while (isalpha(ch) || isdigit(ch) || ch == '_') {
            strToken[i++] = ch;
            ch = pathname[(*ptr)++];
        }

        if(ch=='/'||ch=='\0'){
            strToken[i]='\0'; //识别出一个dir_name
            return DIR_NAME;
        }else{
            return ILLEGAL;//pathname非法
        }

    }else if(ch=='.'){
        strToken[i++]=ch;
        ch=pathname[(*ptr)++];
        if(ch=='.'){
            strToken[i++]=ch;
            ch=pathname[(*ptr)++];
            if(ch=='/'||ch=='\0'){
                strToken[i]='\0';  //识别出 ..
                return DOT_DOT;
            }else
                return ILLEGAL;
        }else if(ch=='/'||ch=='\0'){
            strToken[i]='\0'; //识别出 .
            return DOT;
        }else{
            return ILLEGAL;

        }

    }else if(ch=='/'){
        strToken[i++]=ch;
        strToken[i]='\0'; //识别出一个slash
        return SLASH;
    }else{
        return ILLEGAL;
    }
}
