#include<func.h>
#include<shadow.h>
#include<crypt.h>
#define STR_LEN 12
void  GenerateStr(char*salt) {
    srand(time(NULL));
    strcpy(salt,"$6$");
    for(int i = strlen(salt); i < STR_LEN-1; i++) {
        int flag = rand()%3;
        switch(flag) {
        case 0: salt[i] = rand()%26 + 'a'; break;
        case 1: salt[i] = rand()%26 + 'A'; break;
        case 2: salt[i] = rand()%10 + '0'; break;
        }
    }
    salt[STR_LEN-1]='$';
    printf("%s\n", salt);//输出生成随机字符串
}


int main(int argc, char*argv[]){
    ARGS_CHECK(argc, 2);
    char salt[STR_LEN]={0};
    GenerateStr(salt);
    printf("generate random salt:%s\n",salt);

    char* encrypted=crypt(argv[1],salt);
    printf("password:%s +salt:%s =encrypted:%s\n",argv[1],salt,encrypted);

    printf("len of encrypted:%ld\n",strlen(encrypted));
    return 0;

}
