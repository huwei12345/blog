#include "mysqlpool.h"
#include "addjson.h"
#include "xtest.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>
/*可单测的函数

*/
//完成使用场景的测试

TEST(insert_user, scene)
{

    Status Result=SUCCESS;
/*    User *p =new User;
    p->name=new char[10];
    strcpy(p->name,"robot");
    p->address=NULL;
    p->create_time=NULL;
    p->sex=new char[5];
    strcpy(p->sex,"nan");
    p->article_num=0;
    p->fans_num=0;
    p->account=new char[5];
    strcpy(p->account,"z123");
    p->password=new char[5];
    strcpy(p->password,"z123");
    Result=insert_user(p);
    ASSERT_EQ(Result,SUCCESS);
*/
/*
    User_Relation *rel=new User_Relation;
    rel->rel_user_id=3;
    rel->user_id=2;
    Result=insert_user_rel(rel);
    ASSERT_EQ(Result,SUCCESS);
*/
/*
    Group* group=new Group;
    group->user_id=2;
    group->group_id=5;
    group->father_group_id=0;
    group->group_name=new char[10];
    strcpy(group->group_name,"group3");
    Result=insert_group(group);
    ASSERT_EQ(Result,SUCCESS);
    
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
    Result=insert_article(article);
    ASSERT_EQ(Result,SUCCESS);
    
    Comment* comment=new Comment;
    comment->comment_id=3;
    comment->art_id=1;
    comment->com_user_id=2;
    comment->text=new char[100];
    strcpy(comment->text,"xie de zhen hao");
    comment->upvote_num=0;
    comment->is_question=0;
    Result=insert_comment(comment);
    ASSERT_EQ(Result,SUCCESS);
    
    Collect* collect=new Collect;
    collect->user_id=1;
    collect->collect_art_id=3;
    collect->collect_num=1;
    Result=insert_collect(collect);
    ASSERT_EQ(Result,SUCCESS);
*/
}

//  测试键值对存在的情况
TEST(query_my_user, exist)//函数名。exist
{
/*
    //未释放
    char q[100]="huwei123\0";
    User *p=query_my_user(q,q);
    printf("\nid =%d,name =%s,address=%s,sex=%s,create_time=%s,fans_num=%d,article_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->fans_num,p->article_num);
    ASSERT_EQ(p->user_id,1);
    ASSERT_STRCASEEQ(p->name,"huwei");
    int count=0;
    User_Relation *rel=query_user_rel(1,&count);
    for(int i=0;i<count;i++)
        printf("user_id=%d,user_rel_id=%d\n",rel[i].user_id,rel[i].rel_user_id);
*/
}
TEST(test_s2j, scene)
{
    //User *p =new User;
    //int Result=insert_user(p);
    //数组类型
    char* str=NULL;
    User *user =new User[3];
    User *user1=user;
    user->name=new char[10];
    strcpy(user->name,"robot");
    //user->address=NULL;
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

    user++;
    user->name=new char[10];
    strcpy(user->name,"2num");
    user->article_num=30;

    user++;
    user->name=new char[20];
    strcpy(user->name,"3num");
    user->sex=new char[5];
    strcpy(user->sex,"男");
    user->article_num=3;
    user->fans_num=3;
    
    user=user1;
    str=struct2json(user,USER,3);

    int len_t;
    User* p=(User*)json2struct(str,USER,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,name=%s,art_num=%d,fans_num=%d\n",p[i].user_id,p[i].name,p[i].article_num,p[i].fans_num);



    User_Relation *rel=new User_Relation[2];
    User_Relation *temp=rel;
    rel->rel_user_id=3;
    rel->user_id=2;
    rel++;
    rel->rel_user_id=5;
    rel->user_id=6;
    {
    str=struct2json(temp,USER_REL,2);

    User_Relation* q=(User_Relation*)json2struct(str,USER_REL,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,user_rel_id=%d\n",q[i].user_id,q[i].rel_user_id);
    }
    
    {
    str=struct2json(temp,USER_REL,1);
    User_Relation* q=(User_Relation*)json2struct(str,USER_REL,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,user_rel_id=%d\n",q[i].user_id,q[i].rel_user_id);
    }



    Group* group=new Group[2];
    Group *group1=group;
    group->user_id=2;
    group->group_id=5;
    group->father_group_id=0;
    group->group_name=new char[10];
    strcpy(group->group_name,"group3");
    group++;
    group->user_id=3;
    group->group_name=new char[10];
    strcpy(group->group_name,"group5");
    {
    str=struct2json(group1,GROUP,2);

    Group* q=(Group*)json2struct(str,GROUP,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_name=%s\n",q[i].user_id,q[i].group_name);
    }
    
    {
    str=struct2json(group1,GROUP,1);
    Group* q=(Group*)json2struct(str,GROUP,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_name=%s\n",q[i].user_id,q[i].group_name);
    }


    Article* article=new Article[2];
    Article* art1=article;
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
    article++;
    article->user_id=3;
    article->group_id=4;
    article->upvote_num=10;
    {
    str=struct2json(art1,ARTICLE,2);

    Article* q=(Article*)json2struct(str,ARTICLE,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_id=%d\n",q[i].user_id,q[i].group_id);
    }
    
    {
    str=struct2json(art1,ARTICLE,1);
    Article* q=(Article*)json2struct(str,ARTICLE,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_id=%d\n",q[i].user_id,q[i].group_id);
    }

/*
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
    Collect *collect_cp=(Collect*)json2struct(str,COLLECT,&len_t);
    printf("user_id=%d,collect_art_id=%d,collect_num=%d\n",collect_cp->user_id,collect_cp->collect_art_id,collect_cp->collect_num);
*/
}

TEST(test_mysql2json,scene)
{
    //写从query中查出来的，然后再转
    //写从query中查出来的，然后再转，然后加协议，再测试，黑盒就OK了。只剩删除的依赖检查和一些细节
    //后续再做，下午7点开始做qt的网络，然后接收协议，把json解析出来放入结构体
    char* str=NULL;
    int len_t=0;
    int count_t=0;
    User_Relation* user_rel=query_user_rel(1,&count_t);
    {
    str=struct2json(user_rel,USER_REL,count_t);

    User_Relation* q=(User_Relation*)json2struct(str,USER_REL,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,user_rel_id=%d\n",q[i].user_id,q[i].rel_user_id);
    }
    
    {
    str=struct2json(user_rel,USER_REL,1);
    User_Relation* q=(User_Relation*)json2struct(str,USER_REL,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,user_rel_id=%d\n",q[i].user_id,q[i].rel_user_id);
    }

    Article* article=query_article_title(1,&count_t);
    {
    str=struct2json(article,ARTICLE,count_t);
    Article* q=(Article*)json2struct(str,ARTICLE,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_id=%d,title=%s\n",q[i].user_id,q[i].group_id,q[i].title);
    }
    
    {
    str=struct2json(article,ARTICLE,1);
    Article* q=(Article*)json2struct(str,ARTICLE,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,group_id=%d,title=%s\n",q[i].user_id,q[i].group_id,q[i].title);
    }

    Comment* comment=query_comment(1,&count_t);
    {
    str=struct2json(comment,COMMENT,count_t);
    Comment* q=(Comment*)json2struct(str,COMMENT,&len_t);
    for(int i=0;i<len_t;i++)
        printf("art_id=%d,text=%s\n",q[i].art_id,q[i].text);
    }
    
    {
    str=struct2json(comment,COMMENT,1);
    Comment* q=(Comment*)json2struct(str,COMMENT,&len_t);
    for(int i=0;i<len_t;i++)
        printf("art_id=%d,text=%s\n",q[i].art_id,q[i].text);
    }


    Collect* collect=query_collect(1,&count_t);
    {
    str=struct2json(collect,COLLECT,count_t);
    Collect* q=(Collect*)json2struct(str,COLLECT,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,collect_art_id=%d\n",q[i].user_id,q[i].collect_art_id);
    }
    
    {
    str=struct2json(collect,COLLECT,1);
    Collect* q=(Collect*)json2struct(str,COLLECT,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,collect_art_id=%d\n",q[i].user_id,q[i].collect_art_id);
    }

}

TEST(test_j2s, scene)
{
/*
    printf("---------------------------------------------------------\n");
    char* temp=new char[500];
    strcpy(temp,"[{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0},{\"user_id\":      0,\"name\": \"robot\",\"address\":      \"NULL\",\"sex\":  \"男\",\"create_time\":  \"2021-04-14 01:53:56\",\"article_num\":  0}]");
    int len_t;
    User* p=(User*)json2struct(temp,USER,&len_t);
    for(int i=0;i<len_t;i++)
        printf("user_id=%d name=%s address=%s sex=%s ctime=%s art_num=%d fans_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->article_num,p->fans_num);
*/
    //非数组类型
    char* str=NULL;
    User *user =new User[3];
    user->name=new char[10];
    strcpy(user->name,"robot");
    //user->address=NULL;
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
    for(int i=0;i<len_t;i++)
        printf("user_id=%d,name=%s,art_num=%d,fans_num=%d\n",p[i].user_id,p[i].name,p[i].article_num,p[i].fans_num);


}
int main(int argc, char **argv)
{
    mysql_init();
	return xtest_start_test(argc, argv);
}

//可以记录一下j2p的，再p2j的，再反过来
//要求能够json中的数组转数组，并提取长度
//结构体数组转json数组，不必提取长度
//另外insert里的sprinf是否使用str+strlen(str)作为第一个参数更好，否则插入文章时浪费太多内存
//明天解决这两个问题，后端更加能够使用，并连接qt，在qt中接收实际字符串，解析协议，并接入cJSON
// 2  提取结构体，并用于登录，然后再做注册功能，查询关注、收藏，个人信息。论文第二章简单写一些，ppt简单制作
// 3  中期检查后能够查询文章名，再文章内容，做出qt界面的点击文章名跳转，是否要做数据模型以存储id，而显示名字？
// 3  后端目前只支持一个连接，后续需要创建线程池，数据库连接池，并设计thread类，能过支持多链接。
// 3  再之后可以将C重构为C++，再有时间可以考虑event类型、后端日志，前端界面其他功能。