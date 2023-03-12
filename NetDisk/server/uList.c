#include"uList.h"

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
	
    userNode* pre = NULL;
	if (ptr  != NULL) {
        int user_id=ptr->userId;
		//删除ptr指向的节点
			if (ptr == list->pFront) {//①删除的是头节点
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
			printf("UserFd=%d userid=%d has been deleted from Ulist\n",peerFd,user_id);
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


