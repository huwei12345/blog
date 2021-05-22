#include"addjson.h"
void test_s2j();
void test_j2s();

//将结构体转化为json字符串
char* struct2json(void* obj,type type,int length)
{
    if(obj==NULL)
        return NULL;
    char* str=NULL;
    cJSON *json_obj,*json;
    if(length>1)
        json_obj=cJSON_CreateArray();
    else
        json_obj=cJSON_CreateObject();
    int len=0;
    json=json_obj;
    switch(type)
    {
        case USER:
        {
            User* user=(User*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
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
                if(user->account!=NULL)
                cJSON_AddStringToObject(json_obj,"account",user->account);
                if(user->password!=NULL)
                cJSON_AddStringToObject(json_obj,"password",user->password);
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    user++;
                }
                len++;
            }
            str=cJSON_Print(json);
            {
                if(str)
                {}    //printf("%s\n",str);
            }
            if(json)
                cJSON_Delete(json);
            break;
        }
        case USER_REL:
        {
            User_Relation* rel=(User_Relation*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
                if(rel->user_id!=-1)
                    cJSON_AddNumberToObject(json_obj,"user_id",rel->user_id);
                if(rel->rel_user_id!=-1)
                    cJSON_AddNumberToObject(json_obj,"rel_user_id",rel->rel_user_id);
                if(rel->name!=NULL)
                    cJSON_AddStringToObject(json_obj,"name",rel->name);
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    rel++;
                }
                len++;
            }
            str=cJSON_Print(json);
            {
                if(str)
                {}    //printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case GROUP:
        {
            Group* group=(Group*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
                if(group->user_id!=-1)
                cJSON_AddNumberToObject(json_obj,"user_id",group->user_id);
                if(group->group_id!=-1)
                cJSON_AddNumberToObject(json_obj,"group_id",group->group_id);
                if(group->group_name!=NULL)
                cJSON_AddStringToObject(json_obj,"group_name",group->group_name);
                if(group->father_group_id!=-1)
                cJSON_AddNumberToObject(json_obj,"father_group_id",group->father_group_id);
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    group++;
                }
                len++;
            }
            str=cJSON_Print(json);
            {
                if(str)
                {}    //printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case ARTICLE:
        {
            Article* article=(Article*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
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
                if(article->type!=-1)
                cJSON_AddNumberToObject(json_obj,"type",article->type);
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    article++;
                }
                len++;
            }
            str=cJSON_Print(json);
            {
                if(str)
                {}//    printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case COMMENT:
        {
            Comment* comment=(Comment*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
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
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    comment++;
                }
                len++;

            }
            str=cJSON_Print(json);
            {
                if(str)
                {}    //printf("%s\n",str);
            }
            if(json_obj)
                cJSON_Delete(json_obj);
            break;
        }
        case COLLECT:
        {
            Collect* collect=(Collect*)obj;
            while(len<length)
            {
                if(length>1)
                    json_obj=cJSON_CreateObject();//创建数组中需要的对象
                if(collect->user_id!=-1)
                    cJSON_AddNumberToObject(json_obj,"user_id",collect->user_id);
                if(collect->collect_art_id!=-1)
                    cJSON_AddNumberToObject(json_obj,"collect_art_id",collect->collect_art_id);
                if(collect->collect_num!=-1)
                    cJSON_AddNumberToObject(json_obj,"collect_num",collect->collect_num);
                if(collect->art_name!=NULL)
                    cJSON_AddStringToObject(json_obj,"art_name",collect->art_name);
                if(collect->art_user_id!=-1)
                    cJSON_AddNumberToObject(json_obj,"art_user_id",collect->art_user_id);
                if(length>1)
                {
                    cJSON_AddItemToArray(json,json_obj);//如果是数组，向其中添加对象
                    collect++;
                }
                len++;

            }
            str=cJSON_Print(json);
            {
                if(str)
                {}    //printf("%s\n",str);
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

//将json内容转化为结构体，数组如何？如何知道length
void* json2struct(char* json_str,type type,int *size)
{
    cJSON *json,*json_temp,*json_array;
    json=cJSON_Parse(json_str);

    //printf("is_array=%d\n",cJSON_IsArray(json));
    int length=1;
    int len=0;
    *size=1;
    json_array=json;
    if(cJSON_IsArray(json))
    {
        length=cJSON_GetArraySize(json);
        *size=length;
    }
    switch(type)
    {
        case USER:
        {
            User* user=new User[length];
            User* temp=user;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"user_id");
                    user->user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"name"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"name");
                    user->name=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"address"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"address");
                    user->address=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"sex"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"sex");
                    user->sex=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"create_time"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"create_time");
                    user->create_time=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"article_num"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"article_num");
                    user->article_num=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"fans_num"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"fans_num");
                    user->fans_num=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"account"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"account");
                    user->account=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"password"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"password");
                    user->password=json_temp->valuestring;
                }
                user++;
                len++;
            }
            return temp;
        }
        case USER_REL:
        {
            User_Relation *user_rel=new User_Relation[length];
            User_Relation *temp=user_rel;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"user_id");
                    user_rel->user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"rel_user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"rel_user_id");
                    user_rel->rel_user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"name"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"name");
                    user_rel->name=json_temp->valuestring;
                }
                user_rel++;
                len++;
            }
            return temp;
        }
        case GROUP:
        {
            Group *group=new Group[length];
            Group *temp=group;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"user_id");
                    group->user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"group_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"group_id");
                    group->group_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"father_group_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"father_group_id");
                    group->father_group_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"group_name"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"group_name");
                    group->group_name=json_temp->valuestring;
                }
                group++;
                len++;
            }
            return temp;
        }
        case ARTICLE:
        {
            Article *article=new Article[length];
            Article *temp=article;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"user_id");
                    article->user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"art_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"art_id");
                    article->art_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"title"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"title");
                    article->title=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"text"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"text");
                    article->text=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"upvote_num"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"upvote_num");
                    article->upvote_num=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"create_time"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"create_time");
                    article->create_time=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"modify_time"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"modify_time");
                    article->modify_time=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"group_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"group_id");
                    article->group_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"type"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"type");
                    article->type=json_temp->valueint;
                }
                article++;
                len++;
            }
            return temp;
        }
        case COMMENT:
        {
            Comment *comment=new Comment[length];
            Comment *temp=comment;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"art_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"art_id");
                    comment->art_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"com_user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"com_user_id");
                    comment->com_user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"comment_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"comment_id");
                    comment->comment_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"comment_text"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"comment_text");
                    comment->text=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"upvote_num"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"upvote_num");
                    comment->upvote_num=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"is_question"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"is_question");
                    comment->is_question=json_temp->valueint;
                }
                comment++;
                len++;
            }
            return temp;
        }
        case COLLECT:
        {
            Collect *collect=new Collect[length];
            Collect *temp=collect;
            while(len<length)
            {
                if(length>1)
                    json_array=cJSON_GetArrayItem(json,len);
                if(cJSON_HasObjectItem(json_array,"user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"user_id");
                    collect->user_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"collect_art_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"collect_art_id");
                    collect->collect_art_id=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"collect_num"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"collect_num");
                    collect->collect_num=json_temp->valueint;
                }
                if(cJSON_HasObjectItem(json_array,"art_name"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"art_name");
                    collect->art_name=json_temp->valuestring;
                }
                if(cJSON_HasObjectItem(json_array,"art_user_id"))
                {
                    json_temp=cJSON_GetObjectItem(json_array,"art_user_id");
                    collect->art_user_id=json_temp->valueint;
                }
                collect++;
                len++;
            }
            return temp;
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
    str=struct2json(user,USER,1);
    int len_t;
    User* p=(User*)json2struct(str,USER,&len_t);
    printf("user_id=%d,name=%s,address=%s,sex=%s,ctime=%s,art_num=%d,fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);


    User_Relation *rel=new User_Relation;
    rel->rel_user_id=3;
    rel->user_id=2;
    struct2json(rel,USER_REL,1);


    Group* group=new Group;
    group->user_id=2;
    group->group_id=5;
    group->father_group_id=0;
    group->group_name=new char[10];
    strcpy(group->group_name,"group3");
    str=struct2json(group,GROUP,1);

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
    str=struct2json(article,ARTICLE,1);

    Comment* comment=new Comment;
    comment->comment_id=3;
    comment->art_id=1;
    comment->com_user_id=2;
    comment->text=new char[100];
    strcpy(comment->text,"xie de zhen hao");
    comment->upvote_num=0;
    comment->is_question=0;
    str=struct2json(comment,COMMENT,1);

    Collect* collect=new Collect;
    collect->user_id=1;
    collect->collect_art_id=3;
    collect->collect_num=1;
    str=struct2json(collect,COLLECT,1);
    Collect *collect_cp=(Collect*)json2struct(str,COLLECT,&len_t);
    printf("user_id=%d,collect_art_id=%d,collect_num=%d\n",collect_cp->user_id,collect_cp->collect_art_id,collect_cp->collect_num);
}
void test_j2s()
{
    int len_t;
    char* temp=new char[200];
    strcpy(temp,"{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0}");
    User* p=(User*)json2struct(temp,USER,&len_t);
    printf("user_id=%d name=%s address=%s sex=%s ctime=%s art_num=%d fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);
}

/*
int main()
{
    test_s2j();
    test_j2s();
}*/
