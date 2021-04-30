#include "mysqlpool.h"
#include "xtest.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>
/*可单测的函数

*/
//完成使用场景的测试

TEST(insert_user, scene)
{
    int Result=0;
    /*
    User *p =new User;
    p->name=new char[10];
    strcpy(p->name,"robot");
    p->address=new char[5];
    strcpy(p->address,"NULL");
    p->create_time=new char[20];
    strcpy(p->create_time,"2021-04-14 01:53:56");
    p->sex=new char[5];
    strcpy(p->sex,"nan");
    p->article_num=0;
    p->fans_num=0;
    p->account=new char[5];
    strcpy(p->account,"z123");
    p->password=new char[5];
    strcpy(p->password,"z123");
    int Result=insert_user(p);
    ASSERT_EQ(Result,SUCCESS);

    
    User_Relation *rel=new User_Relation;
    rel->rel_user_id=3;
    rel->user_id=2;
    Result=insert_user_rel(rel);
    ASSERT_EQ(Result,SUCCESS);

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
    */
    Collect* collect=new Collect;
    collect->user_id=1;
    collect->collect_art_id=3;
    collect->collect_num=1;
    Result=insert_collect(collect);
    ASSERT_EQ(Result,SUCCESS);

}

//  测试键值对存在的情况
TEST(query_my_user, exist)//函数名。exist
{
    //未释放
    char q[100]="huwei123\0";
    User *p=query_my_user(q,q);
    printf("\nid =%d,name =%s,address=%s,sex=%s,create_time=%s,fans_num=%d,article_num=%d\n",p->user_id,p->name,p->address,p->sex,p->create_time,p->fans_num,p->article_num);
    ASSERT_EQ(p->user_id,1);
    ASSERT_STRCASEEQ(p->name,"huwei");
}
TEST(modify_user, scene)
{
    //User *p =new User;
    //int Result=insert_user(p);
}
TEST(delete_user, scene)
{
    //User *p =new User;
    //int Result=insert_user(p);
}
int main(int argc, char **argv)
{
    mysql_init();
	return xtest_start_test(argc, argv);
}
