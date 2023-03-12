#include<func.h>
#include"uList.h"

int main(){

//创建并初始化一个list
//show(list);
    userList*list=CreateUserList();

if(!InitUserList(list)){
    printf("error in InitUserList\n");
}
show(list);
AddUser(1,list);
show(list);

DeleteUser(1,list);
show(list);
//添加一个结点后，删除该结点


    return 0;
}
