#ifndef __mysql_H__
#define __mysql_H__
#define USERNAME_MAX 25
#define SALT_LEN  12
#define QUERY_MAX  1024  
#define INT_LEN  10
#define CWD_MAX 100
#define FIME_NAME_MAX 100
#define BIGINT_LEN 20
#define ENCRYPED_LEN 98
int mysql_user(char*query,char *id,char*salt ,char*encrypted);
int mysql_get_cwd(char* query,int*cwd);    
int mysql_file_id(char*query,int *id);
int mysql_file(char*query,int *id,int*pid,char*filename ,int*own_id,char*md5,char*type);
#endif
