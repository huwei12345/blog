#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H
#include<iostream>
#include<stdio.h>
using namespace std;
class CMysql;
enum Status{REQUEST=0,REQ_FORMAT_ERROR,SUCCESS,FAILURE,PERMISSION_DENEY,NET_BROKEN,INSERT_ERROR,GET_MYSQL_ERROR,MODIFY_ERROR,DELETE_ERROR,QUERY_ERROR};
//用户信息
class User
{
public:
    User():user_id(-1),
            name(NULL),
            address(NULL),
            sex(NULL),
            create_time(NULL),
            fans_num(-1),
            article_num(-1),
            account(NULL),
            password(NULL)
    {}
    int user_id;//用户id
    char* name;//昵称
    char* address;//地址
    char* sex;//性别
    char* create_time;//创建日期
    int fans_num;//粉丝数
    int article_num;//文章数
    char* account;//账号
    char* password;//密码

    
};
//用户关系
class User_Relation
{
public:
    User_Relation():user_id(-1),
                    rel_user_id(-1),
                    name(NULL)
    {}
    int user_id;//关注者id
    int rel_user_id;//被关注者id
    char* name;//关注者name
};
//文章分组
class Group
{
public:
    Group():user_id(-1),
            group_id(-1),
            group_name(NULL),
            father_group_id(-1)
    {}
    int user_id;//每个用户都有分组
    int group_id;//组id
    char* group_name;
    int father_group_id;//本组的父组节点
};
//文章
class Article
{
public:
    Article():art_id(-1),
                user_id(-1),
                title(NULL),
                text(NULL),
                upvote_num(-1),
                create_time(NULL),
                modify_time(NULL),
                group_id(-1),
                type(-1)
    {}
    int art_id;//文章id
    int user_id;//用户id
    char* title;//文章名
    char* text;//文章内容
    int upvote_num;//点赞数
    char* create_time;//创建时间
    char* modify_time;//最后修改时间
    int group_id;//属于分组
    int type;//类型
};
//评论
class Comment
{
public:
    Comment():comment_id(-1),
                art_id(-1),
                com_user_id(-1),
                text(NULL),
                upvote_num(-1),
                is_question(-1)
    {}
    int comment_id;//评论id
    int art_id;//文章id
    int com_user_id;//评论者的id
    char* text;//评论内容
    int upvote_num;//评论获赞数
    int is_question;//回复是否针对问题
};
//收藏
class Collect
{
public:
    Collect():user_id(-1),
                collect_art_id(-1),
                collect_num(-1),
                art_user_id(-1),
                art_name(NULL)
    {}
    int user_id;//用户id
    int collect_art_id;//被收藏文章/问题id
    int collect_num;//收藏顺序
    int art_user_id;
    char* art_name;
};

void mysql_init();
CMysql* get_mysql_handler();

User* query_my_user(char* account,char* password);
User* query_user(int user_id);
int query_have_user_account(char* account);//不需要返回数组
User_Relation* query_user_rel(int user_id,int* count);
Group* query_group(int user_id,int* count);
Article* query_article_title(int user_id,int* count);
Article* query_article(int art_id);
Article* query_article_id(int user_id);
Comment* query_comment(int art_id,int* count);
Collect* query_collect(int user_id,int* count);
Group* query_group_id(int user_id);

Status query_user_rel_exist(User_Relation* rel);
Status query_user_col_exist(Collect* col);
Status add_art_upvote(int art_id);

Article* query_article_bytype(int type,int *count);
Article* query_article_bynow(int *count);
Article* query_article_bymonth(int *count);
Article* query_article_byweek(int *count);

Status insert_user(User *p);
Status insert_user_rel(User_Relation *user_relation);
Status insert_group(Group *group);
Status insert_article(Article *article);
Status insert_comment(Comment *comment);
Status insert_collect(Collect *collect);

Status modify_user(User *user);
Status modify_user_rel(User_Relation *user_relation);
Status modify_group(Group *group);
Status modify_article(Article *article);
Status modify_comment(Comment *comment);
Status modify_collect(Collect *collect);

Status delete_user(int user_id);
Status delete_user_rel(int user_id,int user_rel_id);
Status delete_all_user_rel(int user_id);
Status delete_group(int user_id,int group_id);
Status delete_all_group(int user_id);
Status delete_article(int art_id);
Status delete_all_article(int user_id);
Status delete_comment(int art_id,int comment_id);
Status delete_comment_all(int art_id);
Status delete_collect(int user_id,int art_id);
Status delete_collect_all(int user_id);
#endif
