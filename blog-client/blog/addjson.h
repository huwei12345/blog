#ifndef ADD_JSON_H
#define ADD_JSON_H
#include<stdio.h>
#include"cJSON.h"
#include<string.h>
#include<blog.h>
//目前函数只支持对象，不支持对象数组，每类都需要实现数组功能

char* struct2json(void* obj,type type,int length);
void* json2struct(char* json_str,type type,int *size);

/* 
//  void* json2struct(char* json_str,type type);
    request -> get(query ): -> 解析request协议 -> 解析json ,调用 mysql_query()返回结构体 -> 将结构体放入json、再在json前加协议。 返回response,然后发送
            -> set(delete、update、set):mysql_insert()、mysql_delete()、mysql_modify(),
    
*/
#endif
