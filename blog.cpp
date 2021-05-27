#include "blog.h"
int package_request(char* buf,char* str,short type)
{
    //str:json数据
    //strcpy(buf+12,str);
    int length=12;
    if(str!=NULL)
    {
        length=strlen(str)+12+1;
        strncpy(buf+12,str,strlen(str));//赋值str
        buf[length-1]='\0';
        delete[] str;
    }
    short status=0;
    memcpy(buf,&length,4);
    memcpy(buf+4,&status,2);//status
    memcpy(buf+6,&type,2);
    int CRC=0;
    memcpy(buf+8,&CRC,4);
    return length;
}
int package_request2(char* buf,char* str,short type)
{
    //str:json数据
    //strcpy(buf+12,str);
    int length=12;
    if(str!=NULL)
    {
        length=strlen(str)+12+1;
    }
    short status=0;
    memcpy(buf,&length,4);
    memcpy(buf+4,&status,2);//status
    memcpy(buf+6,&type,2);
    int CRC=0;
    memcpy(buf+8,&CRC,4);
    return length;
}
