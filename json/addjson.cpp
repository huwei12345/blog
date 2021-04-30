#include<stdio.h>
#include"cJSON.h"
#include<string.h>
#include"../mysqlpool.h"
enum type{USER=0,USER_REL,GROUP,ARTICLE,COMMENT,COLLECT};
char* struct2json(void* obj,type type);
void* json2struct(char* json_str,type type);
void test_s2j()
{
    User *p =new User;
    p->name=new char[10];
    strcpy(p->name,"robot");
    p->address=new char[5];
    strcpy(p->address,"NULL");
    p->create_time=new char[20];
    strcpy(p->create_time,"2021-04-14 01:53:56");
    p->sex=new char[5];
    strcpy(p->sex,"男");
    p->article_num=0;
    p->fans_num=0;
    p->account=new char[5];
    strcpy(p->account,"z123");
    p->password=new char[5];
    strcpy(p->password,"z123");
    struct2json(p,USER);

    User_Relation *rel=new User_Relation;
    rel->rel_user_id=3;
    rel->user_id=2;
    struct2json(rel,USER_REL);

    Group* group=new Group;
    group->user_id=2;
    group->group_id=5;
    group->father_group_id=0;
    group->group_name=new char[10];
    strcpy(group->group_name,"group3");
    struct2json(rel,GROUP);

    Article* article=new Article;
    article->user_id=1;
    article->group_id=1;
    article->title=new char[10];
    strcpy(article->title,"new robot");
    article->text=new char[100];
    strcpy(article->text,"it is a new robot article");
    article->create_time=new char[20];
    strcpy(article->create_time,"2021-04-14 01:53:56");
    article->modify_time=new char[20];
    strcpy(article->modify_time,"2021-04-14 01:53:56");
    article->upvote_num=0;
    struct2json(article,ARTICLE);

    Comment* comment=new Comment;
    comment->comment_id=3;
    comment->art_id=1;
    comment->com_user_id=2;
    comment->text=new char[100];
    strcpy(comment->text,"xie de zhen hao");
    comment->upvote_num=0;
    comment->is_question=0;
    struct2json(comment,COMMENT);

    Collect* collect=new Collect;
    collect->user_id=1;
    collect->collect_art_id=3;
    collect->collect_num=1;
    struct2json(collect,COLLECT);
}
void test_j2s()
{
    char* temp=new char[200];
    strcpy(temp,"{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0,\"fans_num\":     0}");
    User* p=(User*)json2struct(temp,USER);
    printf("user_id=%d name=%s address=%s sex=%s ctime=%s art_num=%d fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);
}
//将结构体转化为json字符串
char* struct2json(void* obj,type type)
{
    char* str=NULL;
    switch(type)
    {
        case USER:
        {
            User* user=(User*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",user->user_id);
            cJSON_AddStringToObject(json_obj,"name",user->name);
            cJSON_AddStringToObject(json_obj,"address",user->address);
            cJSON_AddStringToObject(json_obj,"sex",user->sex);
            cJSON_AddStringToObject(json_obj,"create_time",user->create_time);
            cJSON_AddNumberToObject(json_obj,"article_num",user->article_num);
            cJSON_AddNumberToObject(json_obj,"fans_num",user->fans_num);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case USER_REL:
        {
            User_Relation* rel=(User_Relation*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",rel->user_id);
            cJSON_AddNumberToObject(json_obj,"rel_user_id",rel->rel_user_id);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case GROUP:
        {
            Group* group=(Group*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",group->user_id);
            cJSON_AddNumberToObject(json_obj,"group_id",group->group_id);
            cJSON_AddStringToObject(json_obj,"group_name",group->group_name);
            cJSON_AddNumberToObject(json_obj,"father_group_id",group->father_group_id);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case ARTICLE:
        {
            Article* article=(Article*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",article->user_id);
            cJSON_AddNumberToObject(json_obj,"art_id",article->art_id);
            cJSON_AddStringToObject(json_obj,"title",article->title);
            cJSON_AddStringToObject(json_obj,"text",article->text);
            cJSON_AddNumberToObject(json_obj,"upvote_num",article->upvote_num);
            cJSON_AddStringToObject(json_obj,"create_time",article->create_time);
            cJSON_AddStringToObject(json_obj,"modify_time",article->modify_time);
            cJSON_AddNumberToObject(json_obj,"group_id",article->group_id);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case COMMENT:
        {
            Comment* comment=(Comment*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",comment->art_id);
            cJSON_AddNumberToObject(json_obj,"com_user_id",comment->com_user_id);
            cJSON_AddNumberToObject(json_obj,"comment_id",comment->comment_id);
            cJSON_AddStringToObject(json_obj,"comment_text",comment->text);
            cJSON_AddNumberToObject(json_obj,"upvote_num",comment->upvote_num);
            cJSON_AddNumberToObject(json_obj,"is_question",comment->is_question);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case COLLECT:
        {
            Collect* collect=(Collect*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            cJSON_AddNumberToObject(json_obj,"user_id",collect->user_id);
            cJSON_AddNumberToObject(json_obj,"collect_art_id",collect->collect_art_id);
            cJSON_AddNumberToObject(json_obj,"collect_num",collect->collect_num);
            str=cJSON_Print(json_obj);
            {
                if(str)
                    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        default:
            return str;
    }
    return str;
}

//将json内容转化为结构体
void* json2struct(char* json_str,type type)
{
    char* str=NULL;
    cJSON *json,*json_temp;
    json=cJSON_Parse(json_str);
    switch(type)
    {
        case USER:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;
        }
        case USER_REL:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;
        }
        case GROUP:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;
        }
        case ARTICLE:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;
        }
        case COMMENT:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;
        }
        case COLLECT:
        {
            User *user=new User;
            json_temp=cJSON_GetObjectItem(json,"user_id");
            user->user_id=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"name");
            user->name=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"address");
            user->address=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"sex");
            user->sex=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"create_time");
            user->create_time=json_temp->valuestring;
            json_temp=cJSON_GetObjectItem(json,"article_num");
            user->article_num=json_temp->valueint;
            json_temp=cJSON_GetObjectItem(json,"fans_num");
            user->fans_num=json_temp->valueint;
            return user;      
        }
    }
    return NULL;
}

int main()
{
    test_s2j();
    test_j2s();
}