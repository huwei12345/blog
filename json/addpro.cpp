#include<stdio.h>
//为json内容添加协议前缀
//最终黑盒数据
char* addprotocol(char* json)
{
    char* protocol;
    char* version;
    char* type;
    char* op_type;
    int length;
    int CRC;
    json[0-64]=length;
    //增加64位前缀，用协议层封装
    return json[0-64];
}