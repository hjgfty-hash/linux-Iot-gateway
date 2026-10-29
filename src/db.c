#include "db.h"
#include<stdio.h>
#include<string.h>

sqlite3* db=NULL;

int callback(void *arg,int column_size,char* column_value[],char *column_name[]){
        char* out_buf=(char*)arg;
        if(column_value[0]&&column_value[1]){
                char tmp[512]={0};
                sprintf(tmp,"%s:%s,%s:%s\n",column_name[0],column_value[0],column_name[1],column_value[1]);
                strcat(out_buf,tmp);
        }
        return 0;
}

void init_db(){
	int ret=sqlite3_open("./student.db",&db);
	if(ret!=SQLITE_OK){
		fprintf(stderr,"sqlite3_open:%s\n",sqlite3_errmsg(db));
		return ;
	}

	char* errmsg=NULL;
	char* sql="create table if not exists student(id integer,name text);";
	sqlite3_exec(db,sql,NULL,NULL,&errmsg);
	if(errmsg) sqlite3_free(errmsg);
}

int db_insert_student(int id,const char* name){
	char sql[512];
	sprintf(sql,"insert into student (id,name) values ('%d','%s')",id,name);
	return sqlite3_exec(db,sql,NULL,NULL,NULL);
}

void db_query_all(char* out_buf,size_t max_len){
	memset(out_buf,0,max_len);
	sqlite3_exec(db,"select * from student;",callback,out_buf,NULL);
}
