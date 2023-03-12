#include<func.h>

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

void show(userList*list);
//peerFd用户成功登陆后，将其登录状态标记为1，并记录其用户id
bool ChangeIsLogin(int peerFd,int userId,userList*list);

//获取peeFd用户的用户id， 返回值（-1 peerfd不存在，  0 peerFd还未登录）
int GetUserId(int peerFd,userList*list);

//销毁Ulist中的mutex, cond
bool DestoryUserList(userList* list);


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



userList* CreateUserList(void){
    return (userList*)calloc(1,sizeof(userList));
}

bool InitUserList(userList* list){
    int ret=pthread_mutex_init(&list->mutex,NULL);
    THREAD_ERROR_CHECK(ret,"pthread_mutex_init");

    ret=pthread_cond_init(&list->cond,NULL);
    THREAD_ERROR_CHECK(ret,"pthread_cond_init");

    return true;
}

void show(userList* list){
    if(list){
        userNode* pNode=list->pFront;
        int cnt=1;
        while(pNode!=NULL){
            printf("%dthNode : peerfd:%d\n",cnt++,pNode->peerFd);
            pNode=pNode->next;
        }


    }else{
        printf("list do not exit\n");
    }
}


userNode* AddUser(int peerFd,userList* list){
    userNode* newNode = (userNode * )malloc(sizeof(userNode));
    if (newNode == NULL) {
        printf("Error: malloc failed in add_before_pFront.\n");
        exit(1);
    }
    newNode->isLogin=0; 
    newNode->peerFd=peerFd;
    newNode->userId=0;


    // 插入
    newNode->next = list->pFront;

    // 修改UserList的属性
    list->pFront = newNode;
    if (list->pRear == NULL) {
        list->pRear = newNode;
    }
    list->userNum++;

    printf("A new UserNode has been added to uList!\n");

    return newNode;
}

userNode*  FindUser(userList* list,int peerFd) {
    userNode* ptr = list->pFront;
    bool isFind = false;
    while (ptr != NULL) {
        if (ptr->peerFd==peerFd) {
            isFind = true;
            break;
        }
        ptr = ptr->next;
    }
    if (isFind) {
        return ptr;
    }
    else {
        return NULL;
    }
}

bool ChangeIsLogin(int peerFd,int userId,userList*list){
    userNode*pNode=FindUser(list,peerFd);
    if(pNode){
        pNode->isLogin=1;
        pNode->userId=userId;
        return true;
    }else{
        printf("cannot find peerFd:%d in Ulist\n",peerFd);
        return false;
    }

}

userNode* DeleteUser(int peerFd,userList* list){
    printf("begin DeleteUser\n");
    userNode* ptr = FindUser(list, peerFd);
    printf("After find  DeleteUser: ptr=%p\n",ptr);
    
    userNode* pre = NULL;
    if (ptr  != NULL) {
       int userid=ptr->userId;
        //删除ptr指向的节点
        if (ptr == list->pFront) {//①删除的是头节点
            printf("删除的是头节点\n");
            //list->pFront = NULL;
            //list->pRear = NULL;

            if(list->pFront==list->pRear){
                //删除的头节点是链表中的唯一一个结点
                list->pFront=list->pRear=NULL;
            }else{
                list->pFront=ptr->next;
            }

            list->userNum--;
            free(ptr);
            ptr = NULL;
        }
        else {
            //pre指向pre的前驱节点
            printf("删除的不是头节点\n");
            pre = list->pFront;
            while (pre->next != ptr)
                pre = pre->next;

            pre->next = ptr->next;//从链表中删除pre指向的节点
            if (ptr == list->pRear)//②删除的是尾结点时需要修改pRear
                list->pRear = pre;
            free(ptr);//释放节点pre指向节点的空间
            ptr = NULL;
            list->userNum--;
        }
        printf("UserFd=%d userid=%d has been deleted from Ulist\n",peerFd,userid);
        if (pre&&pre->next != NULL)
            return pre->next;
        else
            return NULL;
    }//if 链表中存在目标结点
    else { //else 链表中不存在目标结点
        printf("UserFd%d don't exist!\n",peerFd);
        return NULL;
    }

}

//获取peeFd用户的用户id， 返回值（-1 peerfd不存在，  0 peerFd还未登录）
int GetUserId(int peerFd,userList*list){
    userNode*ptr=FindUser(list,peerFd);
    if(ptr!=NULL){
        if(ptr->isLogin)
            return ptr->userId;
        else
            return 0;

    }else
        return -1;
}
void DeleteList(userList* list) {

    while (list->pFront != NULL) {
        userNode* temp = list->pFront;
        list->pFront = list->pFront->next;
        free(temp);
    }
    free(list);
}

//销毁Ulist中的mutex, cond
bool DestoryUserList(userList* list){
    if(list){
        //销毁用户链表中的同步锁，互斥锁       
        int ret=pthread_mutex_destroy(&list->mutex);
        THREAD_ERROR_CHECK(ret,"pthread_mutex_init");

        ret=pthread_cond_destroy(&list->cond);
        THREAD_ERROR_CHECK(ret,"pthread_cond_init");

        //释放用户链表中动态申请的空间
        DeleteList(list);
    }
    return true;
}


