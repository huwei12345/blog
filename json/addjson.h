//function 这是json转化结构体，结构体转json的函数，支持数组，特定支持6个结构体，可扩展
//comment
//date 2021.5.27
//author @huwei
//other 
#ifndef ADD_JSON_H
#define ADD_JSON_H
#include<stdio.h>
#include"cJSON.h"
#include<string.h>
#include"mysqlpool.h"
enum type{USER=0,USER_REL,GROUP,ARTICLE,COMMENT,COLLECT};
char* struct2json(void* obj,type type,int length);
void* json2struct(char* json_str,type type,int *size);
#endif