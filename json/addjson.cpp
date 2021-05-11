#include<stdio.h>
#include"cJSON.h"
#include<string.h>
#include"../mysqlpool.h"
#include"addjson.h"
void test_s2j();
void test_j2s();

//将结构体转化为json字符串
char* struct2json(void* obj,type type)
{
    if(obj==NULL)
        return NULL;
    char* str=NULL;
    switch(type)
    {
        case USER:
        {
            User* user=(User*)obj;
            cJSON* json_obj;
            json_obj=cJSON_CreateObject();
            if(user->user_id!=-1)
                cJSON_AddNumberToObject(json_obj,"user_id",user->user_id);
            if(user->name!=NULL)
            cJSON_AddStringToObject(json_obj,"name",user->name);
            if(user->address!=NULL)
                cJSON_AddStringToObject(json_obj,"address",user->address);
            if(user->sex!=NULL)
                cJSON_AddStringToObject(json_obj,"sex",user->sex);
            if(user->create_time!=NULL)
            cJSON_AddStringToObject(json_obj,"create_time",user->create_time);
            if(user->article_num!=-1)
            cJSON_AddNumberToObject(json_obj,"article_num",user->article_num);
            if(user->fans_num!=-1)
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
            if(rel->user_id!=-1)
                cJSON_AddNumberToObject(json_obj,"user_id",rel->user_id);
            if(rel->rel_user_id!=-1)
                cJSON_AddNumberToObject(json_obj,"rel_user_id",rel->rel_user_id);
            if(rel->name!=NULL)
                cJSON_AddStringToObject(json_obj,"name",rel->name);
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
            if(group->user_id!=-1)
            cJSON_AddNumberToObject(json_obj,"user_id",group->user_id);
            if(group->group_id!=-1)
            cJSON_AddNumberToObject(json_obj,"group_id",group->group_id);
            if(group->group_name!=NULL)
            cJSON_AddStringToObject(json_obj,"group_name",group->group_name);
            if(group->father_group_id!=-1)
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
            if(article->user_id!=-1)
            cJSON_AddNumberToObject(json_obj,"user_id",article->user_id);
            if(article->art_id!=-1)
            cJSON_AddNumberToObject(json_obj,"art_id",article->art_id);
            if(article->title!=NULL)
            cJSON_AddStringToObject(json_obj,"title",article->title);
            if(article->text!=NULL)
            cJSON_AddStringToObject(json_obj,"text",article->text);
            if(article->upvote_num!=-1)
            cJSON_AddNumberToObject(json_obj,"upvote_num",article->upvote_num);
            if(article->create_time!=NULL)
            cJSON_AddStringToObject(json_obj,"create_time",article->create_time);
            if(article->modify_time!=NULL)
            cJSON_AddStringToObject(json_obj,"modify_time",article->modify_time);
            if(article->group_id!=-1)
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
            if(comment->art_id!=-1)
            cJSON_AddNumberToObject(json_obj,"art_id",comment->art_id);
            if(comment->com_user_id!=-1)
            cJSON_AddNumberToObject(json_obj,"com_user_id",comment->com_user_id);
            if(comment->comment_id!=-1)
            cJSON_AddNumberToObject(json_obj,"comment_id",comment->comment_id);
            if(comment->text!=NULL)
            cJSON_AddStringToObject(json_obj,"comment_text",comment->text);
            if(comment->upvote_num!=-1)
            cJSON_AddNumberToObject(json_obj,"upvote_num",comment->upvote_num);
            if(comment->is_question!=-1)
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
            if(collect->user_id!=-1)
                cJSON_AddNumberToObject(json_obj,"user_id",collect->user_id);
            if(collect->collect_art_id!=-1)
                cJSON_AddNumberToObject(json_obj,"collect_art_id",collect->collect_art_id);
            if(collect->collect_num!=-1)
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
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                user->user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"name"))
            {
                json_temp=cJSON_GetObjectItem(json,"name");
                user->name=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"address"))
            {
                json_temp=cJSON_GetObjectItem(json,"address");
                user->address=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"sex"))
            {
                json_temp=cJSON_GetObjectItem(json,"sex");
                user->sex=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"create_time"))
            {
                json_temp=cJSON_GetObjectItem(json,"create_time");
                user->create_time=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"article_num"))
            {
                json_temp=cJSON_GetObjectItem(json,"article_num");
                user->article_num=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"fans_num"))
            {    
                json_temp=cJSON_GetObjectItem(json,"fans_num");
                user->fans_num=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"account"))
            {
                json_temp=cJSON_GetObjectItem(json,"account");
                user->create_time=json_temp->valuestring; 
            }
            if(cJSON_HasObjectItem(json,"password"))
            {
                json_temp=cJSON_GetObjectItem(json,"password");
                user->create_time=json_temp->valuestring;
            }
            return user;
        }
        case USER_REL:
        {
            User_Relation *user_rel=new User_Relation;
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                user_rel->user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"rel_user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"rel_user_id");
                user_rel->rel_user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"name"))
            {
                json_temp=cJSON_GetObjectItem(json,"name");
                user_rel->name=json_temp->valuestring;
            }
            return user_rel;
        }
        case GROUP:
        {
            Group *group=new Group;
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                group->user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"group_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"group_id");
                group->group_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"father_group"))
            {
                json_temp=cJSON_GetObjectItem(json,"father_group");
                group->father_group_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"group_name"))
            {
                json_temp=cJSON_GetObjectItem(json,"group_name");
                group->group_name=json_temp->valuestring;
            }
            return group;
        }
        case ARTICLE:
        {
            Article *article=new Article;
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                article->user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"art_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"art_id");
                article->art_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"title"))
            {
                json_temp=cJSON_GetObjectItem(json,"title");
                article->title=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"text"))
            {
                json_temp=cJSON_GetObjectItem(json,"text");
                article->text=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"upvote_num"))
            {
                json_temp=cJSON_GetObjectItem(json,"upvote_num");
                article->upvote_num=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"create_time"))
            {
                json_temp=cJSON_GetObjectItem(json,"create_time");
                article->create_time=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"modify_time"))
            {
                json_temp=cJSON_GetObjectItem(json,"modify_time");
                article->modify_time=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"group_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"group_id");
                article->group_id=json_temp->valueint;
            }
            return article;
        }
        case COMMENT:
        {
            Comment *comment=new Comment;
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                comment->art_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"com_user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"com_user_id");
                comment->com_user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"comment_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"comment_id");
                comment->comment_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"comment_text"))
            {
                json_temp=cJSON_GetObjectItem(json,"comment_text");
                comment->text=json_temp->valuestring;
            }
            if(cJSON_HasObjectItem(json,"upvote_num"))
            {
                json_temp=cJSON_GetObjectItem(json,"upvote_num");
                comment->upvote_num=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"is_question"))
            {
                json_temp=cJSON_GetObjectItem(json,"is_question");
                comment->is_question=json_temp->valueint;
            }
            return comment;
        }
        case COLLECT:
        {
            Collect *collect=new Collect;
            if(cJSON_HasObjectItem(json,"user_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"user_id");
                collect->user_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"collect_art_id"))
            {
                json_temp=cJSON_GetObjectItem(json,"collect_art_id");
                collect->collect_art_id=json_temp->valueint;
            }
            if(cJSON_HasObjectItem(json,"collect_num"))
            {
                json_temp=cJSON_GetObjectItem(json,"collect_num");
                collect->collect_num=json_temp->valueint;
            }
            return collect;      
        }
    }
    return NULL;
}

void test_s2j()
{
    char* str=NULL;

    User *user =new User;
    user->name=new char[10];
    strcpy(user->name,"robot");
    user->address=new char[5];
    strcpy(user->address,"NULL");
    user->create_time=new char[20];
    strcpy(user->create_time,"2021-04-14 01:53:56");
    user->sex=new char[5];
    strcpy(user->sex,"男");
    user->article_num=1;
    user->fans_num=1;
    user->account=new char[5];
    strcpy(user->account,"z123");
    user->password=new char[5];
    strcpy(user->password,"z123");
    str=struct2json(user,USER);
    
    User* p=(User*)json2struct(str,USER);
    printf("user_id=%d,name=%s,address=%s,sex=%s,ctime=%s,art_num=%d,fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);


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
    str=struct2json(group,GROUP);

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
    str=struct2json(article,ARTICLE);

    Comment* comment=new Comment;
    comment->comment_id=3;
    comment->art_id=1;
    comment->com_user_id=2;
    comment->text=new char[100];
    strcpy(comment->text,"xie de zhen hao");
    comment->upvote_num=0;
    comment->is_question=0;
    str=struct2json(comment,COMMENT);

    Collect* collect=new Collect;
    collect->user_id=1;
    collect->collect_art_id=3;
    collect->collect_num=1;
    str=struct2json(collect,COLLECT);
    Collect *collect_cp=(Collect*)json2struct(str,COLLECT);
    printf("user_id=%d,collect_art_id=%d,collect_num=%d\n",collect_cp->user_id,collect_cp->collect_art_id,collect_cp->collect_num);
}
void test_j2s()
{
    char* temp=new char[200];
    strcpy(temp,"{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0}");
    User* p=(User*)json2struct(temp,USER);
    printf("user_id=%d name=%s address=%s sex=%s ctime=%s art_num=%d fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);
}

/*
int main()
{
    test_s2j();
    test_j2s();
}*/
