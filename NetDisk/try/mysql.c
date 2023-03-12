#include<func.h>
#include<mysql/mysql.h>

MYSQL*conn=NULL;
const char *host="localhost";
const char *user="root";
const char *passwd="confetti";
const char *db="netDiskDB";

int mysql_file(char*query,int *id,int*pid,char*filename ,int*own_id,char*md5,char*type){
    //1) 初始化mysql的连接句柄
    conn=mysql_init(NULL);
    //2) 建立连接
    if(mysql_real_connect(conn,host,user,passwd,db,0,NULL,0)==NULL){
        printf("error:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }

    //3) 执行查询
    int ret=mysql_query(conn,query);
    if(ret){//query出了问题
        printf("error query1:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }else{
        //4) 获取查询结果集
        MYSQL_RES *result=mysql_store_result(conn);
        if(result==NULL){
            if(mysql_field_count(conn)==0){
                //query不是select语句，不会返回任何数据
                printf("Query OK,%ld row affected\n",mysql_affected_rows(conn));
            }else{
                //获取查询返回结果时出错
                printf("error query2:%s\n",mysql_error(conn));
                return EXIT_FAILURE;
            }
        }else{
            //成功获取到了query的结果
            int rows=mysql_num_rows(result);
            int cols=mysql_num_fields(result);
            printf("rows=%d,cols=%d\n",rows,cols);
            // printf("rows:%d, cols:%d\n",rows,cols);
            if(rows==0){
                //query结果为空
                printf("Empty set\n");
            }else{

                //5) 获取每一行数据
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))!=NULL){
                   
                    for(int i=0;i<cols;i++){
                    printf("%10s ",row[i]);
                    }
                   if(id!=NULL){
                   *id=atoi(row[0]);
                   }
                   if(pid!=NULL){
                   *pid=atoi(row[1]);
                   }
                   if(filename!=NULL){
                   strcpy(filename,row[2]);
                   }
                   if(own_id!=NULL){
                       *own_id=atoi(row[3]);
                   }
                   if(md5!=NULL){
                       strcpy(md5,row[4]);
                   }if(type!=NULL){
                       strcpy(type,row[5]);
                   }

                    printf("\n");
                }
                printf("%d row in set\n",rows);
            }
        }


        //释放结果集
        mysql_free_result(result);
        //关闭连接
        mysql_close(conn);
    }

    return 0;
}

int mysql_file_id(char*query,int *id){
    //1) 初始化mysql的连接句柄
    conn=mysql_init(NULL);
    //2) 建立连接
    if(mysql_real_connect(conn,host,user,passwd,db,0,NULL,0)==NULL){
        printf("error:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }

    //3) 执行查询
    int ret=mysql_query(conn,query);
    if(ret){//query出了问题
        printf("error query1:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }else{
        //4) 获取查询结果集
        MYSQL_RES *result=mysql_store_result(conn);
        if(result==NULL){
            if(mysql_field_count(conn)==0){
                //query不是select语句，不会返回任何数据
                printf("Query OK,%ld row affected\n",mysql_affected_rows(conn));
            }else{
                //获取查询返回结果时出错
                printf("error query2:%s\n",mysql_error(conn));
                return EXIT_FAILURE;
            }
        }else{
            //成功获取到了query的结果
            int rows=mysql_num_rows(result);
            int cols=mysql_num_fields(result);
            // printf("rows:%d, cols:%d\n",rows,cols);
            if(rows==0){
                //query结果为空
                printf("Empty set\n");
            }else{
                //5) 获取每一行数据
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))!=NULL){
                   printf("%10s ",row[0]);
                   if(id!=NULL){
                   *id=atoi(row[0]);
                   }
                    printf("\n");
                }
                printf("%d row in set\n",rows);
            }
        }
        //释放结果集
        mysql_free_result(result);
        //关闭连接
        mysql_close(conn);
    }

    return 0;
}
int mysql_user(char*query,char *id,char*salt ,char*encrypted){
    //1) 初始化mysql的连接句柄
    conn=mysql_init(NULL);
    //2) 建立连接
    if(mysql_real_connect(conn,host,user,passwd,db,0,NULL,0)==NULL){
        printf("error:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }

    //3) 执行查询
    int ret=mysql_query(conn,query);
    if(ret){//query出了问题
        printf("error query1:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }else{
        //4) 获取查询结果集
        MYSQL_RES *result=mysql_store_result(conn);
        if(result==NULL){
            if(mysql_field_count(conn)==0){
                //query不是select语句，不会返回任何数据
                printf("Query OK,%ld row affected\n",mysql_affected_rows(conn));
            }else{
                //获取查询返回结果时出错
                printf("error query2:%s\n",mysql_error(conn));
                return EXIT_FAILURE;
            }
        }else{
            //成功获取到了query的结果
            int rows=mysql_num_rows(result);
            int cols=mysql_num_fields(result);
            // printf("rows:%d, cols:%d\n",rows,cols);
            if(rows==0){
                //query结果为空
                printf("Empty set\n");
            }else{

                //5) 获取每一行数据
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))!=NULL){
                   printf("%10s ",row[0]);
                   printf("%10s ",row[1]);
                   printf("%10s",row[2]);
                   if(id!=NULL){
                   strcpy(id,row[0]);
                   }
                   if(id!=NULL){
                   strcpy(salt,row[1]);
                   }
                   if(id!=NULL){
                   strcpy(encrypted,row[2]);
                   }
                    printf("\n");
                }
                printf("%d row in set\n",rows);
            }
        }


        //释放结果集
        mysql_free_result(result);
        //关闭连接
        mysql_close(conn);
    }

    return 0;
}

int mysql_get_cwd(char*query,int*cwd){
    //1) 初始化mysql的连接句柄
    conn=mysql_init(NULL);
    //2) 建立连接
    if(mysql_real_connect(conn,host,user,passwd,db,0,NULL,0)==NULL){
        printf("error:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }

    //3) 执行查询
    int ret=mysql_query(conn,query);
    if(ret){//query出了问题
        printf("error query1:%s\n",mysql_error(conn));
        return EXIT_FAILURE;
    }else{
        //4) 获取查询结果集
        MYSQL_RES *result=mysql_store_result(conn);
        if(result==NULL){
            if(mysql_field_count(conn)==0){
                //query不是select语句，不会返回任何数据
                printf("Query OK,%ld row affected\n",mysql_affected_rows(conn));
            }else{
                //获取查询返回结果时出错
                printf("error query2:%s\n",mysql_error(conn));
                return EXIT_FAILURE;
            }
        }else{
            //成功获取到了query的结果
            int rows=mysql_num_rows(result);
            int cols=mysql_num_fields(result);
            // printf("rows:%d, cols:%d\n",rows,cols);
            if(rows==0){
                //query结果为空
                printf("Empty set\n");
            }else{

                //5) 获取每一行数据
                MYSQL_ROW row;
                while((row=mysql_fetch_row(result))!=NULL){
                   printf("%10s ",row[0]);
                   if(cwd!=NULL){
                       *cwd=atoi(row[0]);
                   }
                    printf("\n");
                }
                printf("%d row in set\n",rows);
            }
        }


        //释放结果集
        mysql_free_result(result);
        //关闭连接
        mysql_close(conn);
    }

    return 0;
}
