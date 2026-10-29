#ifndef DB_H
#define DB_H
#include<sqlite3.h>
#include <stddef.h>

extern sqlite3* db;
void init_db();
int db_insert_student(int id,const char* name);
void db_query_all(char* out_buf,size_t max_len);
#endif
