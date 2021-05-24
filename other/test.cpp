#include<iostream>
#include<string.h>
using namespace std;
class A
{
    public:
    A():b(NULL),c(1){}
    char* b;
    int c;
};
class User
{
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
    int user_id;//关注者id
    int rel_user_id;//被关注者id
    char* name;//关注者name
};
//文章分组
class Group
{
public:
    int user_id;//每个用户都有分组
    int group_id;//组id
    char* group_name;
    int father_group_id;//本组的父组节点
};
//文章
class Article
{
public:
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
    int user_id;//用户id
    int collect_art_id;//被收藏文章/问题id
    int collect_num;//收藏顺序
    int art_user_id;
    char* art_name;
};
int main()
{
    printf("\u0017⚌\u000b");
    /*
    char *str=new char[1000];
    strcpy(str,"hello");
    for(int i=0;i<10;i++)
    {
        a->c=i;
        a->b=str;
        a++;
    }
    a=b;
    for(int i=0;i<10;i++)
    {
        printf("%d\n",a->c);
        printf("%s\n",a->b);
        a++;
    }
        snprintf(str,100,"hello world");
        snprintf(str+strlen(str),100,"this is my world");
        snprintf(str+strlen(str),100,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        snprintf(str+strlen(str),100,"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    printf("str=%s\n",str);

    char* name=new char[10];
    strcpy(name,"男");
    for(int i=0;i<strlen(name);i++)
    {
        printf("%c ",name[i]);
    }
    */

    return 0;
}