#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<pthread.h>
#include<func.h>
bool isdir(struct dirent* pdirent){

    return DT_DIR==pdirent->d_type&&strcmp(pdirent->d_name, ".")&&strcmp(pdirent->d_name, "..");
}


void DFS(const char* dirname, int depth){

    printf("opendir %s\n",dirname);
    //打开目录流dirname
    DIR*pdir=opendir(dirname);  
    ERROR_CHECK(pdir,NULL,"opendir");
    struct dirent* pdirent=readdir(pdir);


    //遍历dirname的目录项，若某一个目录项为目录，则递归进行上述过程
    while(true){
        if(pdirent==NULL) break;

        //如果当前目录项是一个目录（不包括. ..）
        //则递归地遍历该目录
        if(isdir(pdirent)){ 
            //打印depth个Tab ,然后再打印name
            for(int i=0;i<depth;i++)
                printf("    ");
            puts(pdirent->d_name);

            // dirname/d_name\0
            char* newdirname=(char*)calloc(strlen(dirname)+strlen(pdirent->d_name)+2,sizeof(char));
            //若使用malloc 分配未初始化的空间，则后续的strcat操作会出现未定义的行为
            strcat(newdirname,dirname);
            strcat(newdirname,"/");
            strcat(newdirname,pdirent->d_name);

            printf("DFS %s\n",newdirname);
            DFS(newdirname, depth+1);
            free(newdirname);
            pdirent=readdir(pdir);
        }else{//当前目录项不是一个目录

            if(strcmp(pdirent->d_name,"..")&&strcmp(pdirent->d_name,".")){//当前目录项不是目录也不是. ..
                //打印depth个Tab ,然后再打印name
                for(int i=0;i<depth;i++)
                    printf("    ");
                puts(pdirent->d_name);
                //当前目录项地相对路劲入队

            }
            pdirent=readdir(pdir);
        }
    }//while
    closedir(pdir);
}
