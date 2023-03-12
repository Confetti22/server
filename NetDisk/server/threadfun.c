#include<func.h>
#include"threadfun.h"
#include"parser.h"
#include"uList.h"
#include"tcp.h"
#include"mysql.h"
#include"md5.h"
#define MaxCOMMANDBUF  128  //client发来的指令的大小上限

extern const int THREADNUM; //线程池中线程数量
extern int exitpipe[2];//用于发送，接受 SIGUSR1 (自定义的退出信号)
extern thread_pool_t threadpool;//线程池（共享资源）
extern userList * puList;//用户链表指针

void Handle_events(int epfd,struct epoll_event* events,int nready,int listenfd){
    //遍历就绪队列，依次处理就绪事件
    for(int i=0;i<nready;i++){
        int fd=events[i].data.fd;
        //根据描述符类型和事件类型进程处理
        if(fd==listenfd){
            Handle_accept(epfd,listenfd);
        }else if(fd==exitpipe[0]) {
            Handle_exit(epfd,exitpipe[0]);
        }else{
            Handle_command(epfd,fd);
        }
    }
}

void Handle_accept(int epfd,int listenfd){
    printf("*********main_thr begin Handle_accept*********\n");
    int peerfd=accept(listenfd,NULL,NULL);
    if(peerfd<0){
        perror("accept");
    }
    printf(" accepte a new conn, peerfd:%d\n",peerfd);

    //添加peerfd到 epoll读监听事件中
    EpollAdd(epfd,peerfd,EPOLLIN);
    //添加peerfd到用户链表中，islgoin=0,userid=0, peerfd=peerfd
    AddUser(peerfd,puList);
    printf("*********main_thr end Handle_accept*********\n");
}

//管道exitpipe[0]管道可读，表明父进程收到了退出信号
int Handle_exit(int epfd, int exitpipe){
    printf("*********main_thr begin Handle_exit*********\n");
    char exitflag;
    read(exitpipe, &exitflag, 1);
    printf("main_thr ready to exit.\n");
    //收到退出的通知之后，子线程要逐步退出
    
    //设置任务队列的退出标志位，并唤醒所有等待在条件变量cond上的子线程
    queueWakeup(&threadpool.queue);

    //主线程等待子线程死亡并为其收尸
    for(int j = 0; j < THREADNUM; ++j) {
        pthread_join(threadpool.threads[j], NULL);
    }

    printf("all sub_threads in  pool had  exited\n");

    //销毁threadpool中申请的系统资源
    threadpoolDestroy(&threadpool);

    //主线程退出
    exit(0);
    printf("cannot see****main_thr end Handle_exit*********\n");

}


void Handle_command(int epfd,int peerfd){
    printf("*********main_thr begin Handle_command*********\n");
    char COMMANDBUF[MaxCOMMANDBUF]={0};
    //接受客户端发来的指令
    int ret=recv(peerfd,COMMANDBUF,MaxCOMMANDBUF,0);
    ERROR_CHECK(ret,-1,"recv command"); 
    printf("recved command:%s\n",COMMANDBUF);
    //解析指令类型
    int op_code;
    char op_para[PARAMAXLEN]={0};
    task_handler_t pfunc=NULL;
    getOPTION_CODE(COMMANDBUF,&op_code,op_para);
    printf("deciphered op_type:%d\n",op_code);
    switch(op_code){
    case 0: pfunc=handle_register;break;
    case 1: pfunc=handle_login;break;
    case 2: pfunc=handle_puts;break;
    case 3: pfunc=handle_gets;break;
    case 4: pfunc=handle_cd;break;
    case 5: pfunc=handle_ls;break;
    case 6: pfunc=handle_rm;break;
    case 7: pfunc=handle_mkdir;break;
    case 8: pfunc=handle_pwd;break;
            //当client输入exit 或是client挂掉后epoll不断就绪，
            //此时就要先删除对该事件的监听
    case 9: pfunc=handle_exit; break;

            //读取到非法指令时直接返回
    default:printf("illegal op_code\n");return ;
    }

    //将新任务添加到任务队列中
    taskEnqueue(&(threadpool.queue),peerfd,pfunc,(void*)op_para);

    //删除对该peerfd的读监听,随后交由子线程与peerfd沟通
    //待子线程处理完毕后，子线程再负责将peerfd重新添加epoll读就绪监听列表中
    EpollDel(epfd,peerfd);
    printf("*********main_thr end of Handle Command**********\n");

}



void* handle_register(void * arg){
    printf(".........child_thr :begin handle_register.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=(arguments->peerfd);
    char*UserName=arguments->commandpara;
    char id[INT_LEN+1]={0};
    char salt[SALT_LEN+1]={0};
    char encrypted[ENCRYPED_LEN+1]={0};

    //query 中的内容      SELECT salt from UserTable where username ='UserName' 
    char query[QUERY_MAX]={0};
    strcat(query,"SELECT id,salt,encrypted from UserTable WHERE username= '");
    strcat(query,UserName);
    strcat(query,"'");
    //只需要获取salt
    int ret=mysql_user(query,NULL,salt,NULL);
    ERROR_CHECK(ret,-1,"mysql_select_UserName");
    // printf("id:%s salt:%s  encrypted:%s\n\n",id,salt,encrypted);

    //send(salt)  当UserName不存在时， salt=NULL
    ret=send(peerfd,salt,SALT_LEN, MSG_WAITALL);

    //UserName不存在时才可以进行注册
    if(salt[0]==0){
        ret=recv(peerfd,encrypted,ENCRYPED_LEN ,MSG_WAITALL);
        ERROR_CHECK(ret,-1,"recv encrypted");

        //从encrypted中提取前12字节作为salt
        strncpy(salt,encrypted,SALT_LEN);

        //INSERT INTO UserTable (username,salt,encrypted) VALUES('uname','salt','encrypted');
        memset(query,0,QUERY_MAX);
        strcat(query,"INSERT INTO UserTable(username,salt,encrypted) VALUES('");
        strcat(query,UserName);
        strcat(query,"','");
        strcat(query,salt);
        strcat(query,"','");
        strcat(query,encrypted);
        strcat(query,"')");
        printf("query:%s\n",query);
        ret=mysql_user(query,NULL,NULL,NULL);
        ERROR_CHECK(ret,-1,"mysql_insert_UserName");

        //修改用户链表中该结点的信息
        memset(query,0,QUERY_MAX);
        strcat(query,"SELECT id,salt,encrypted from UserTable WHERE username= '");
        strcat(query,UserName);
        strcat(query,"'");
        //只需要获取salt
        int ret=mysql_user(query,id,salt,encrypted);
        ERROR_CHECK(ret,-1,"mysql_select_UserName");
        //   printf("id:%s salt:%s  encrypted:%s\n\n",id,salt,encrypted);
        userNode* pNode=FindUser(puList,peerfd);
        pNode->isLogin=1;
        pNode->userId=atoi(id);

    }else{
        printf("UserName has already exit\n");
    }

    printf(".........child_thr :end handle_Register.........\n");
    return NULL;
}





void* handle_login(void*arg){
    printf(".........child_thr :begin handle_login.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=(arguments->peerfd);
    //printf("peerfd:%d, arg.peerfd:%d, arg.arg:%s\n",peerfd,arguments->peerfd,arguments->commandpara);
    char*UserName=arguments->commandpara;
    char id[INT_LEN+1]={0};
    int userid=0;
    char salt[SALT_LEN+1]={0};
    char true_encrypted[ENCRYPED_LEN+1]={0};
    char test_encrypted[ENCRYPED_LEN+1]={0};

    //query 中的内容      SELECT salt from UserTable where username ='UserName' 
    char query[QUERY_MAX]={0};
    strcat(query,"SELECT id,salt,encrypted from UserTable WHERE username= '");
    strcat(query,UserName);
    strcat(query,"'");
    int ret=mysql_user(query,id,salt,true_encrypted);
    ERROR_CHECK(ret,-1,"mysql_select_UserName");
    //printf("id:%s salt:%s  encrypted:%s\n\n",id,salt,true_encrypted);

    //send(salt)  当UserName不存在时， salt=NULL
    ret=send(peerfd,salt,SALT_LEN, MSG_WAITALL);

    if(salt[0]!=0){
        ret=recv(peerfd,test_encrypted,ENCRYPED_LEN ,MSG_WAITALL);
        ERROR_CHECK(ret,-1,"recv test_encrypted");

        int islogin=0;
        if(strcmp(test_encrypted,true_encrypted)==0){
            islogin=1;//匹配成功
            //匹配成功后，修改对应的用户节点的islogin=1 userId=id(从UserTable中查找)
            userid=atoi(id);
            userNode* pNode=FindUser(puList,peerfd);
            pNode->isLogin=1;
            pNode->userId=userid;
            printf("password match success\n");
        }
        else{
            islogin=0;//匹配失败
            printf("password match fail\n");
        }
        //给客服端发送密码匹配结果
        ret=send(peerfd,&islogin,sizeof(islogin),MSG_WAITALL);
        ERROR_CHECK(ret,-1,"send loginstate");
    }else{
        printf("UserName don't exit\n");
    }

    printf(".........child_thr :end handle_login.........\n");
    return NULL;
}





void* handle_puts(void * arg){
    printf(".........child_thr :begin handle_puts.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=arguments->peerfd;
    char md5[MD5_STR_LEN+1]={0};
    strcpy(md5,arguments->commandpara);
    
    char pathname [128]={0};
    strcat(pathname,"file/");
    strcat(pathname,md5);
    //查找本地是否存在md5文件，并给client发送该文件是否存在的信息
    bool isExited=false;
    int filefd=open(pathname,O_RDONLY);
    if(filefd==-1&&errno==ENOENT){
      printf("server do not have this file\n");

    }else if(filefd==-1){
        ERROR_CHECK(filefd,-1,"open");
    }else{//该文件在本地已存在
       printf("server already have this file\n");
       isExited=true;
    }
    int ret=send(peerfd,&isExited,sizeof(bool),0);
    ERROR_CHECK(ret,-1,"send isExited");
    printf("isExited=%d, ret=%d\n",isExited,ret);

    if(!isExited){//不存在时 从client端接受该文件，并写入本地
        recvFile(peerfd,NULL);
        printf("file:%s has been recved and stored\n",md5);
    }

    //先在当前目录下查找该是否存在该文件条目，不存在时，增加一条
    char query[QUERY_MAX]={0};
       //1)从Ulist中查到当前peerfd的user_id 以及str_user_id
       int user_id=GetUserId(peerfd,puList);                               
       char str_user_id[INT_LEN]={0};
       sprintf(str_user_id,"%d",user_id);
      int cwd=-1;
      char str_cwd[INT_LEN]={0};
     Getcwd(user_id,&cwd);
    sprintf(str_cwd,"%d",cwd);
    strcat(query,"select id from FileTable where owner_id=");
    strcat(query,str_user_id);
    strcat(query," and parent_id=");
    strcat(query,str_cwd);
    strcat(query," and md5='");
    strcat(query,md5);
    strcat(query,"'");
    int fileid=0;
     ret=mysql_file_id(query,&fileid);
     ERROR_CHECK(ret,-1,"mysql_file_id find if file has exited");
     if(!fileid){//不存在该文件条目时才进行插入一条新条目
         memset(query,0,QUERY_MAX);
         strcat(query,"INSERT INTO FileTable (parent_id,filename,owner_id,md5,type)VALUE(");
         strcat(query,str_cwd);
         strcat(query,",");
         strcat(query,filename);
         strcat(query,",");
         strcat(query,str_user_id);
         strcat(query,",");
         strcat(query,md5);
         strcat(query,",");
         strcat(query,"'f'");
         strcat(query,")");
        ret=mysql_file_id(query,NULL);
        ERROR_CHECK(ret,-1,"mysql_file_id inset a new record");
     }


    printf("..........child_thr :end handle_puts..........\n");
    return NULL;
}

void* handle_gets(void * arg){
    printf(".........child_thr :begin handle_gets.........\n");
    myarg_t* arguments=(myarg_t*)arg;


    printf(".........child_thr :end handle_gets.........\n");
    return NULL;
}





void* handle_cd(void * arg){
    printf(".........child_thr :begin handle_cd.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=arguments->peerfd;
    char pathname[MaxCOMMANDBUF]={0};
    strcpy(pathname,arguments->commandpara);
    char query[QUERY_MAX]={0}; //存放mysql 查询语句
    char sendbuf[BUFSIZE]={0};

    //1)从Ulist中查到当前peerfd的user_id 以及str_user_id
    int user_id=GetUserId(peerfd,puList);
    char str_user_id[INT_LEN]={0};
    sprintf(str_user_id,"%d",user_id);
    //2)查UserTable 获取用户的当前目录到cwd
    int cwd=-1;
    Getcwd(user_id,&cwd);


    if(cd(pathname,user_id,&cwd)){//cd 命令正确执行后，修改UserTable的cwd 并返回给client执行结果
        //修改UserTable中的cwd
       
        char str_cwd[INT_LEN]={0};
        sprintf(str_cwd,"%d",cwd);
       
        memset(query,0,QUERY_MAX);
        strcat(query,"UPDATE UserTable set cwd=");
        strcat(query,str_cwd);
        strcat(query," WHERE id=");
        strcat(query,str_user_id);
        printf("query:%s\n",query);
       int  ret=mysql_file_id(query,NULL);//使得当前目录向上走一层
        ERROR_CHECK(ret,-1,"mysql_file_id");
        printf("has saved cwd %d into the UserTable\n",cwd);

        if(0==cwd){
            strcpy(sendbuf,"/  \n");
        }else{
            memset(query,0,QUERY_MAX);
            strcat(query,"SELECT filename FROM FileTable WHERE id=");
            strcat(query,str_cwd);
            printf("query:%s\n",query);
            ret=mysql_file_filename(query,sendbuf);
            ERROR_CHECK(ret,-1,"mysql_file");
            printf("after query, sendbuf=%s\n",sendbuf);
            strcat(sendbuf,"\n");
        }
    }else{
        //cd 命令执行失败  向用户发送“cd : fail\n”
        strcpy(sendbuf,"fail\n");

    }

    send_sendbuf(peerfd,sendbuf);

    printf(".........child_thr :end handle_cd.........\n");
    return NULL;
}

void* handle_rm(void * arg){
    printf(".........child_thr :begin handle_rm.........\n");
    myarg_t* arguments=(myarg_t*)arg;


    printf(".........child_thr :end handle_rm.........\n");
    return NULL;
}
void* handle_ls(void * arg){
    printf(".........child_thr :begin handle_ls.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=arguments->peerfd;
    char pathname[MaxCOMMANDBUF]={0};
    strcpy(pathname,arguments->commandpara);
    char sendbuf[BUFSIZE]={0};

    //1)从Ulist中查找到peerfd的id号
    int user_id=GetUserId(peerfd,puList);
    char str_user_id[INT_LEN]={0};
    sprintf(str_user_id,"%d",user_id);

    //1)从UserTable中获取cwd
    int cwd=-1;
    Getcwd(user_id,&cwd);
    bool is_cd_success=true;
    //若ls 后面有参数 则先通过执行cd  根据执行结果 修改cwd
    //若ls 后面没有参数 则无需修改cwd
    if(strlen(pathname)!=0){
        if(cd(pathname,user_id,&cwd)){  //cd 执行成功后,cwd为目标cwd
        }else{
        is_cd_success=false;//cd 执行失败后，要给client发送fail
        }
    }

    //2)从FileTable中查找当前cwd下的所有子文件名称，都保存到sendbuf中
    //（文件名之间用' '分隔）
    //SELECT filename FROM FileTable where owner_id=user_id 
    //and parent_id=cwq 

    if(is_cd_success){
    char query[QUERY_MAX]={0};
    char str_cwd[INT_LEN]={0};
    sprintf(str_cwd,"%d",cwd);
    strcat(query,"SELECT filename from FileTable where owner_id=");
    strcat(query,str_user_id);
    strcat(query," and parent_id=");
    strcat(query,str_cwd);
    int ret=mysql_file_ls(query,sendbuf);
    ERROR_CHECK(ret,-1,"mysql_file_ls");
    printf("after query,sendbuf=%s\n",sendbuf);
    }else{
        strcat(sendbuf,"fail\n");
    }
    //给client发送ls 的结果
    send_sendbuf(peerfd,sendbuf);

    printf(".........child_thr :end handle_ls.........\n");
    return NULL;
}


void* handle_mkdir(void * arg){
    printf(".........child_thr :begin handle_mkdir.........\n");
    myarg_t* arguments=(myarg_t*)arg;


    printf(".........child_thr :end handle_mikdir.........\n");
    return NULL;
}

void* handle_pwd(void * arg){
    printf("..........child_thr :begin handle_pwd.........\n");
    myarg_t* arguments=(myarg_t*)arg;


    printf(".........child_thr :end handle_pwd.........\n");
    return NULL;
}

void* handle_exit(void * arg){
    printf(".........child_thr :begin handle_exit.........\n");
    myarg_t* arguments=(myarg_t*)arg;
    int peerfd=arguments->peerfd;
    //1)从uList中删除该结点
    DeleteUser(peerfd,puList);
    //2）关闭peerfd
    close(peerfd);
    printf(".........child_thr :end handle_exit.........\n");
    return NULL;
}

bool cd(char* pathname,const int user_id, int * pcwd){
    char strToken[128]={0};//用于存放每次从pathname中解析出的符号单位
    char query[QUERY_MAX]={0}; 
    bool  correct=1;//在查询目录的过程中，当某一步出错后，correct=false ，退出循环，终止cd命令执行
    char str_cwd[INT_LEN]={0};
    sprintf(str_cwd,"%d",*pcwd);
    char str_user_id[INT_LEN]={0};
    sprintf(str_user_id,"%d",user_id);
    int NewCWD=-1;

    for(int ptr=0;correct&&pathname[ptr]!='\0';){
        switch(GeneTkcode(pathname,strToken,&ptr)){
        case DOT:{
                     printf("a dot->do nothing\n");
                     ;break; //do nothing
                 }
        case DIR_NAME:{
                          printf("a dir_name:%s\n",strToken);
                          memset(query,0,QUERY_MAX);
                          strcat(query,"SELECT id FROM FileTable WHERE owner_id=");
                          strcat(query,str_user_id);
                          strcat(query," and parent_id=");
                          strcat(query,str_cwd);
                          strcat(query," and filename='");
                          strcat(query,strToken);
                          strcat(query,"'");
                          strcat(query," and type='d'");
                          printf("query:%s\n",query);
                          int ret=mysql_file_id(query,&NewCWD);
                          ERROR_CHECK(ret,-1,"mysql_file_id");
                          if(NewCWD!=-1){
                              *pcwd=NewCWD;
                              printf("find the dir_name,now cwd=%d\n",NewCWD);
                          }else{
                              correct=0;
                              printf("not find the dir_name,cwd is not changed\n");
                          }
                          break;}
        case DOT_DOT:{
                         printf("a dot_dot\n");
                         if(*pcwd!=0){//当前不在根目录时可进入上一层目录
                             memset(query,0,QUERY_MAX);
                             strcat(query,"SELECT parent_id FROM FileTable WHERE owner_id=");
                             strcat(query,str_user_id);
                             strcat(query," and id=");
                             strcat(query,str_cwd);
                             printf("before mysql_query,cwd=%d\n",*pcwd);
                            int ret=mysql_file_id(query,pcwd);//使得当前目录向上走一层
                             printf("after  mysql_query,cwd=%d\n",*pcwd);
                             ERROR_CHECK(ret,-1,"mysql_file_id");
                         }
                         break;}
        case SLASH:{//将当前目录设置为0
                       printf("a SLASH, set cwd=0");
                       *pcwd=0;
                       break;}
        default:{//解析处理非法的目录结构
                    printf("cannot decipher the pathname,abort\n");
                    correct=0;
                }
        }
        memset(strToken,0,sizeof(strToken));
        sprintf(str_cwd,"%d",*pcwd); // 每次cwd更新后，也要更新str_cwd
    }
    return correct;
}


//)查UserTable 获取用户的当前目录到cwd
// SELECT cwd FROM UserTable where id= userid
int Getcwd(const int user_id,int *pcwd){
    char query[QUERY_MAX]={0};
    char str_user_id[INT_LEN]={0};
    sprintf(str_user_id,"%d",user_id);

    strcat(query,"SELECT cwd from UserTable WHERE id=");
    strcat(query,str_user_id);
    int ret=mysql_get_cwd(query,pcwd);
    printf("user's cwd:%d\n",*pcwd);
    ERROR_CHECK(ret,-1,"mysql_get_cwd");
    return 0;
}


//peerfd--->user_id, str_user_id
int get_str_user_id(int peerfd,char* str_user_id){
    //1)从Ulist中查到当前peerfd的user_id 以及str_user_id
    int user_id=GetUserId(peerfd,puList);
    sprintf(str_user_id,"%d",user_id);
    return 0;
}
