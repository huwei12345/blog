#ifndef MYSQLPOOL_H
#define MYSQLPOOL_H
#include<iostream>
#include<stdio.h>
using namespace std;
class CMysql;
enum SQL_Rusult {SUCCESS=0,FAILURE,PERMISSION_DENEY,NET_BROKEN,INSERT_ERROR,GET_MYSQL_ERROR,MODIFY_ERROR};
//用户信息
struct User
{
    unsigned int user_id;//用户id
    char* name;//昵称
    char* address;//地址
    char* sex;//性别
    char* create_time;//创建日期
    unsigned int fans_num;//粉丝数
    unsigned int article_num;//文章数
    char* account;//账号
    char* password;//密码
};
//用户关系
struct User_Relation
{
    unsigned int user_id;//关注者id
    unsigned int rel_user_id;//被关注者id
};
//文章分组
struct Group
{
    unsigned int user_id;//每个用户都有分组
    unsigned int group_id;//组id
    char* group_name;
    unsigned int father_group_id;//本组的父组节点
};
//文章
struct Article
{
    unsigned int art_id;//文章id
    unsigned int user_id;//用户id
    char* title;//文章名
    char* text;//文章内容
    unsigned int upvote_num;//点赞数
    char* create_time;//创建时间
    char* modify_time;//最后修改时间
    unsigned int group_id;//属于分组
};
//评论
struct Comment
{
    unsigned int comment_id;//评论id
    unsigned int art_id;//文章id
    unsigned int com_user_id;//评论者的id
    char* text;//评论内容
    unsigned int upvote_num;//评论获赞数
    unsigned int is_question;//回复是否针对问题
};
//收藏
struct Collect
{
    unsigned int user_id;//用户id
    unsigned int collect_art_id;//被收藏文章/问题id
    unsigned int collect_num;//收藏顺序
};

void mysql_init();
CMysql* get_mysql_handler();

SQL_Rusult insert_user(struct User *p);
SQL_Rusult insert_user_rel(struct User_Relation *user_relation);
SQL_Rusult insert_group(struct Group *group);
SQL_Rusult insert_article(struct Article *article);
SQL_Rusult insert_comment(struct Comment *comment);
SQL_Rusult insert_collect(struct Collect *collect);

User* query_my_user(char* account,char* password);
User* query_user(unsigned int user_id);
User_Relation* query_user_rel(unsigned int user_id);
Group* query_group(unsigned int user_id);
Article* query_article_title(unsigned int user_id);
Article* query_article(unsigned int art_id);
Comment* query_comment(unsigned int art_id);
Collect* query_collect(unsigned int user_id);

SQL_Rusult modify_user(struct User *user);
SQL_Rusult modify_user_rel(struct User_Relation *user_relation);
SQL_Rusult modify_group(struct Group *group);
SQL_Rusult modify_article(struct Article *article);
SQL_Rusult modify_comment(struct Comment *comment);
SQL_Rusult modify_collect(struct Collect *collect);

SQL_Rusult delete_user(int user_id);
SQL_Rusult delete_user_rel(int user_id,int user_rel_id);
SQL_Rusult delete_all_user_rel(int user_id);
SQL_Rusult delete_group(int user_id,int group_id);
SQL_Rusult delete_all_group(int user_id);
SQL_Rusult delete_article(int art_id);
SQL_Rusult delete_all_article(int user_id);
SQL_Rusult delete_comment(int art_id,int comment_id);
SQL_Rusult delete_comment_all(int art_id);
SQL_Rusult delete_collect(int user_id,int art_id);
SQL_Rusult delete_collect_all(int user_id);
#endif