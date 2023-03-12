#ifndef __uList_H__
#define __uList_H__
#include<func.h>



#define MaxUserName 25
#define MaxUserPassword 25

typedef struct user_s{
    bool isLogin;
    int peerFd;
    int userId;
    struct user_s*next;
}userNode;

typedef struct userList_t_s{
    userNode* pFront;
    userNode* pRear;
    int userNum;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}userList;
/**定义用户的增删改查
* 用链表实现用户结点的增删改查，
* #查找#   返回该用户是否存在，若存在，则打印用户姓名和权限
* #修改#   先查找该用户是否存在， 若存在，则修改该用户信息
* #增加一个结点#  ——头插法，返回值为是否插入成功，插入成功则显示一条信息
* #删除特定用户结点
*/

userList* CreateUserList(void);
bool InitUserList(userList* list);
userNode* FindUser(userList*list,int peerFd);
userNode* AddUser(int peerFd,userList* list);
userNode* DeleteUser(int peerFd,userList* list);

//peerFd用户成功登陆后，将其登录状态标记为1，并记录其用户id
bool ChangeIsLogin(int peerFd,int userId,userList*list);

//获取peeFd用户的用户id， 返回值（-1 peerfd不存在，  0 peerFd还未登录）
int GetUserId(int peerFd,userList*list);

//销毁Ulist中的mutex, cond
bool DestoryUserList(userList* list);



#endif
