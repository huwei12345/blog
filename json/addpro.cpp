#include<stdio.h>
#include<string.h>
#include"addpro.h"
//为json内容添加协议前缀
//仅测试用，正式程序没用
//最终黑盒数据
/*
                       4字节位
|                                                  |
------------ -------------- ------------ -----------
                        长度
          状态                        操作类型
                        CRC(针对json+上述4项)                        


*/


//get_black_data,第二个参数也可以是一个channel，能够识别那条通路
//在协议中可能存在\0,所以协议部分单独发送，内容跟在后面
char* addprotocol(char* json,int id,unsigned short type,unsigned short sta)
{
    //4个字段
    char* protocol=NULL;
    unsigned short op_type=type;//操作类型，各个表、各个操作（增删改查）
    unsigned short status=sta;//返回类型，SUNCCESS、Failure。。。
    int CRC=4;//CRC校验，包括json+4个字段,暂无
    int user_id=id;
    int json_length=strlen(json);
    protocol=new char[json_length+16+1];
    protocol[json_length+16]='\0';
    unsigned int length=json_length+16;//包总长度

    strncpy(protocol+16,json,json_length);
    memcpy(protocol,&length,4);
    memcpy(protocol+4,&op_type,2);
    memcpy(protocol+6,&status,2);
    memcpy(protocol+8,&user_id,4);
    memcpy(protocol+12,&CRC,4);
    //增加64位前缀，用协议层封装
    return protocol;
}
void test()
{
     char* s=addprotocol("{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0,\"fans_num\":     0}",1,1,1);
    int len=strlen(s);
    printf("len= %d\n",len);
    for(int i=0;i<16;i++)
    {
        printf("%d ",s[i]);
    }
    printf("%s",s+16);
}
/*
int main()
{
    test();
    
    int x=2;
    char* s=(char*)&x;
    char m[10];
    memcpy(m,&x,4);
    for(int i=0;i<4;i++)
    printf("%d  ",m[i]);
    
    return 0;
}*/