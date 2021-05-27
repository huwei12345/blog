#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "cJSON.h"

int create_js(void)
{
    cJSON *root,*js_body;
    root=cJSON_CreateArray();
    cJSON_AddItemToArray(root,cJSON_CreateString("Hello World"));
    cJSON_AddItemToArray(root,cJSON_CreateNumber(10));
    {
        char *s=cJSON_PrintUnformatted(root);
        if(s)
        {
            printf(" %s\n",s);
            free(s);
        }
    }
    if(root)
        cJSON_Delete(root);
    return 0;
}
int create_js2(void)
{
    cJSON *root,*js_body,*js_list;
    root=cJSON_CreateObject();
    cJSON_AddItemToObject(root,"body",js_body=cJSON_CreateArray());
    cJSON_AddItemToArray(js_body,js_list=cJSON_CreateObject());
    cJSON_AddStringToObject(js_list,"name","fengxin");
    cJSON_AddNumberToObject(js_list,"status",100);
    {
        char* s=cJSON_PrintUnformatted(root);
        if(s)
        {
            printf("%s\n",s);
            free(s);
        }
    }
    if(root)
        cJSON_Delete(root);
    return 0;
}
int main()
{
    cJSON *usr;
    cJSON *arry;

    usr=cJSON_CreateObject();
    cJSON_AddStringToObject(usr,"name","fengxin");
    cJSON_AddStringToObject(usr,"passwd","123");
    cJSON_AddNumberToObject(usr,"num",1);  //加整数
    char *out=cJSON_Print(usr);
    printf("%s\n",out);

    cJSON_Delete(usr);

    create_js();
    create_js2();

    cJSON *json,*json_name,*json_passwd,*json_num;
    char* out1="{\"name\":\"fengxin\",\"passwd\":\"123\",\"num\":1}";
    json=cJSON_Parse(out);
    json_name=cJSON_GetObjectItem(json,"name");
    json_passwd=cJSON_GetObjectItem(json,"passwd");
    json_num=cJSON_GetObjectItem(json,"num");
    printf("name:%s,passwd:%s,num:%d\n",json_name->valuestring,json_passwd->valuestring,json_num->valueint);
    cJSON_Delete(json);
    free(out);
    return 0;
}