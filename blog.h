#ifndef BLOG_H
#define BLOG_H
#include<stdio.h>
#include<string.h>
enum Status{
    REQUEST=0,
    REQ_FORMAT_ERROR,
    SUCCESS,
    FAILURE,
    PERMISSION_DENEY,
    NET_BROKEN,
    INSERT_ERROR,
    GET_MYSQL_ERROR,
    MODIFY_ERROR,
    DELETE_ERROR,
    QUERY_ERROR
    };
//所有的enum都不能改动顺序，因为和前端是保持一致的，加功能需要在前后端同意修改
enum op_type{
    login_t=0,//登录
    query_user_t,//查询用户
    query_user_rel_t,//查询用户关系
    query_user_rel_simple_t,//查询单条用户关系，界面初始化
    query_group_t,//查询分组
    query_article_t,//查询文章
    query_article_title_t,//查询文章title列表
    query_comment_t,//查询评论
    query_collect_t,//查询收藏
    query_collect_simple_t,//查询单条收藏，界面初始化

    query_article_bytype_t,//按类型查询文章
    query_art_bynow_t,//查询最近一天的文章
    query_art_bymonth_t,//查询最近一月的文章
    query_art_byweek_t,//查询最近一周的文章
    query_user_id_t,//按id查询用户
    query_user_name_t,//按name查询用户
    query_article_id_t,//按文章id查询文章
    query_article_name_t,//按文章名查询文章

    insert_user_t,//插入用户
    insert_article_t,//插入文章
    insert_group_t,//插入分组
    insert_collect_t,//插入收藏
    insert_comment_t,//插入评论
    insert_user_rel_t,//插入用户关系

    modify_user_t,//修改用户
    modify_article_t,//修改文章
    modify_group_t,//修改分组
    modify_user_rel_t,//修改用户关系
    modify_comment_t,//修改评论
    modify_article_group_t,//修改文章分组
    add_art_upvote_t,//点赞

    delete_user_t,//删除用户，未解决依赖
    delete_user_rel_t,//删除用户关系
    delete_group_t,//删除分组，内有文章或分组有提示错误
    delete_article_t,//删除文章
    delete_article_list_t,//删除全部文章，未实现
    delete_comment_t,//删除评论，只有文章的用户才能删除
    delete_comment_list_t,//删除评论列表，未实现
    delete_collect_t,//删除收藏
    delete_collect_list_t,//删除收藏列表，未实现

    test//测试
    };
enum type{USER=0,USER_REL,GROUP,ARTICLE,COMMENT,COLLECT};
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
int package_request(char* buf,char* str,short type);
int package_request2(char* buf,char* str,short type);
#endif // BLOG_H
