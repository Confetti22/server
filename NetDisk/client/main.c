#include<func.h>
#include<termio.h>
#include<shadow.h>
#include<crypt.h>
#include"parser.h"
#include"md5.h"
#include"tcp.h"
#define IP "127.0.0.1"
#define PORT 8888

#define MaxUserName 25
#define MaxUserPassword 25

#define MAXCOMMAND 128
#define BUFSIZE  4096

#define SALT_LEN  12  //盐值字符串的长度
#define ENCRYPTED_LEN 98

char UserName[MaxUserName]={0};
char pwd[MaxUserPassword]={0};
int state=-1;



int GetConnected(void);

int getch(void);  //自己实现的输入不回显函数
//读取用户输入
void Login(int clientfd);

/*登录函数，匹配用户输入的密码是否正确，并返回该用户权限
  return value : -1 用户名不存在 -0 密码错误   1 登录成功*/
int CheckPassword(int clientfd,char*UserName,char*pwd);


void  Register(int clientfd);
/*注册一个新用户，并以新注册的用户身份登录到远程网盘服务器*/


int CheckUserName(int clientfd,char*UserName,char*pwd);


void clientputs(int clientfd,char*parabuf);
//随机生成一个12个字符的盐值串，放入salt
void  GenerateSalt(char*salt);

int main(int argc, char*argv[]){
    int clientfd=GetConnected();


SHOW_MENU:
    {
        printf("***********************************************\n");
        printf("**********welcom to use Young_NetDisk**********\n");
        printf("          1.Login\n");
        printf("          2.Register\n");
        printf("select a number :");
        int option;
        scanf("%d",&option);

        switch(option){
        case 1 : Login(clientfd);break;
        case 2 : Register(clientfd);break;
        default:goto SHOW_MENU ;//从新选择操作 （1 or2 ）
        }

    }//END_OF SHOW_MENU


    printf("Hello!  you have successfully login\n");

    //while(1) 从键盘读入用户输入，根据命令形式，选择不同的方式与服务器交互
    while(true){
        char commandbuf[MAXCOMMAND]={0};
        char parabuf[MAXCOMMAND]={0};
       
        //从键盘读入用户输入的命令到commandbuf
        read(STDIN_FILENO,commandbuf,MAXCOMMAND);
        int commandtype=getClientCommandType(commandbuf,parabuf);
        switch(commandtype){
        case -1: break;//非法输入
        case CLNTPUTS: clientputs(clientfd,parabuf);break;
        case CLNTGETS: 




                       break;
        case CLNTEXIT:



                       break;
        case CLNTSHORT: 
                       {
                       int  ret=send(clientfd,commandbuf,strlen(commandbuf),MSG_WAITALL);
                       ERROR_CHECK(ret,-1,"send shot command");
                         
                       char recvbuf[BUFSIZE]={0};
                       //接受server端的命令执行结果（封装在小火车中）
                       int length;
                       ret=recv(clientfd,&length,4,MSG_WAITALL);
                       printf(">>1. result length:%d\n",length);

                       ret=recv(clientfd,recvbuf,length,MSG_WAITALL);
                       printf(">>2. recv result:%s\n",recvbuf);
                       
                       
                       write(STDOUT_FILENO,recvbuf,ret);
                       break;

                       }


        }

    }


    close(clientfd);
    return 0;
}

void clientputs(int clientfd,char*parabuf){

    char md5_str[MD5_STR_LEN+1]={0};
    char sendbuf[BUFSIZE]={0};
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

        //给server 发送 puts  md5
        strcat(sendbuf,"puts ");
        strcat(sendbuf,md5_str);
        ret=send(clientfd,sendbuf,strlen(sendbuf),MSG_WAITALL);
        ERROR_CHECK(ret,-1,"send md5");

        //接受服务器端该文件是否存在的消息
        bool isExited=false;
        recv(clientfd,&isExited,1,MSG_WAITALL);
        if(!isExited){
            //发送文件(采用小火车，分两次发送， 文件名，文件内容)   
            sendFile(clientfd,parabuf);

            printf("file:%s has been uploaded\n",parabuf);
        }
    }
}


void  Register(int clientfd){

    do {
        //读入用户名
        printf("enter user_name:");
        scanf("%s", UserName);
        //printf("you have input UserName:%s\n",UserName);
        //读入密码（隐藏用户输入的密码）
       // clrscr();// To clear the screen 
        bool password = false;
        getchar();//提取输入缓冲区中遗留的换行符
        printf("enter user_password:");
        int i = 0;
        for (i = 0; i < MaxUserPassword; i++) {
            // Get the hidden input using getch() method 
            pwd[i] = getch();

            if (pwd[i] == '\r') {
                pwd[i] = '\0';
                password = true;
                break; //输入换行符密码输入结束
            }
            else if (pwd[i] == '\b') {
                printf("\b \b");
                if (i != 0)i--;
                i--;
            }
            else {
                // Print * to show that a character is entered 
                printf("*");
            }

        }
        if (password) {  //密码长度合法时，匹配用户名与密码，并返回状态码
            printf("\n"); 
            state = CheckUserName(clientfd, UserName, pwd);
        }

    } while (state!=1);
}


/*登录函数，匹配用户输入的密码是否正确，并返回该用户权限
  return value : -1 用户名已存在   1 登录成功
  */
int CheckUserName(int clientfd,char*UserName,char*pwd){


    //send  REGISTER UserName
    char buf[64]={0};
    strcat(buf,"REGISTER ");
    strncat(buf,UserName,strlen(UserName));
    int ret=send(clientfd,buf,strlen(UserName)+9, MSG_WAITALL);
    ERROR_CHECK(ret,-1,"send UserName");

    //recv UserName's salt ( salt!=NULL时 返回-1 )
    char salt[SALT_LEN+1]={0};
    ret= recv(clientfd,salt,SALT_LEN, MSG_WAITALL);
    ERROR_CHECK(ret,-1,"recv salt");
    if(salt[0]!=0){
        printf("UserName:%s  already exit\n",UserName);
        return -1;// 用户名不存在
    }else{
        //genereate encrypt then send encrypted
        GenerateSalt(salt);//随机生成一个盐值
        char* encrypted=crypt((const char*)pwd,(const char*)salt);
        ret=send(clientfd,encrypted,ENCRYPTED_LEN, MSG_WAITALL);
        ERROR_CHECK(ret,-1,"send encrypted");
        return 1;
    }
}

void Login(int clientfd){

    do {
        //读入用户名
        printf("enter user_name:");
        scanf("%s", UserName);
        //读入密码（隐藏用户输入的密码）
        //clrscr();// To clear the screen 
        getchar();//提取输入缓冲区中遗留的换行符
        bool password = false;
        printf("enter user_password:");
        int i = 0;
        for (i = 0; i < MaxUserPassword; i++) {
            // Get the hidden input using getch() method 
            pwd[i] = getch();

            if (pwd[i] == '\r') {
                pwd[i] = '\0';
                password = true;
                break; //输入换行符密码输入结束
            }
            else if (pwd[i] == '\b') {
                printf("\b \b");
                if (i != 0)i--;
                i--;
            }
            else {
                // Print * to show that a character is entered 
                printf("*");
            }

        }
        if (password) {  //密码长度合法时，匹配用户名与密码，并返回状态码
            printf("\n"); 
            state = CheckPassword(clientfd, UserName, pwd);
        }

    } while (state!=1);
}



/*登录函数，匹配用户输入的密码是否正确，并返回该用户权限
  return value : -1 用户名不存在  0 密码错误   1 登录成功
  */

int CheckPassword(int clientfd,char*UserName,char*pwd){


    //send  LOGIN UserName
    char buf[64]={0};
    strcat(buf,"LOGIN ");
    strncat(buf,UserName,strlen(UserName));
    int ret=send(clientfd,buf,strlen(UserName)+6, MSG_WAITALL);
    ERROR_CHECK(ret,-1,"send UserName");

    //recv UserName's salt ( salt==NULL时 返回-1 )
    char salt[SALT_LEN+1]={0};
    ret= recv(clientfd,salt,SALT_LEN, MSG_WAITALL);
    ERROR_CHECK(ret,-1,"recv salt");
    if(salt[0]==0){
        printf("%s don't exit\n",UserName);
        return -1;// 用户名不存在
    }else{
        //genereate encrypt then send encrypted
        char* encrypted=crypt((const char*)pwd,(const char*)salt);
        ret=send(clientfd,encrypted,ENCRYPTED_LEN, MSG_WAITALL);
        ERROR_CHECK(ret,-1,"send encrypted");

        //recv matching result( 1 登录成功   0 密码错误 )
        int islogin = -1;

        ret=recv(clientfd,&islogin,sizeof(islogin), MSG_WAITALL);
        ERROR_CHECK(ret,-1,"recv islogin");

        if(islogin){
            printf("login success!\n");
            return 1;
        }
        else{
            printf("wrong password\n");
            return 0;

        } 
    }
}


int GetConnected(void){
    //1. 创建套接字
    int clientfd =  socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(clientfd, -1, "socket");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(IP);

    //2. 向服务器发起建立连接的请求
    int ret = connect(clientfd, 
                      (struct sockaddr *)&serveraddr, 
                      sizeof(serveraddr));
    ERROR_CHECK(ret, -1, "connect");
    printf("connect has completed.\n");
    return clientfd;
}

int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;

    if (tcgetattr(fd, &tm) < 0) {//保存现在的终端设置
        return -1;
    }

    tm_old = tm;
    cfmakeraw(&tm);//更改终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    if (tcsetattr(fd, TCSANOW, &tm) < 0) {//设置上更改之后的设置
        return -1;
    }

    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0) {//更改设置为最初的样子
        return -1;
    }

    return ch;
}


//随机生成一个12个字符的盐值串，放入salt
void  GenerateSalt(char*salt) {
    srand(time(NULL));
    strcpy(salt,"$6$");
    for(int i = strlen(salt); i < SALT_LEN-1; i++) {
        int flag = rand()%3;
        switch(flag) {
        case 0: salt[i] = rand()%26 + 'a'; break;
        case 1: salt[i] = rand()%26 + 'A'; break;
        case 2: salt[i] = rand()%10 + '0'; break;
        }
    }
    salt[SALT_LEN-1]='$';
    printf("%s\n", salt);//输出生成随机字符串
}

