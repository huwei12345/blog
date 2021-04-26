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
    //User *p =new User;
    //int Result=insert_user(p);
}

//  测试键值对存在的情况
TEST(query_my_user, exist)//函数名。exist
{
    char q[100]="huwei123\0";
    User *p=query_my_user(q,q);
    printf("id =%d name =%s\n",p->user_id,p->name);
    ASSERT_EQ(p->user_id,1);
    ASSERT_STRCASEEQ(p->name,"weiwei");
}

int main(int argc, char **argv)
{
    mysql_init();
	return xtest_start_test(argc, argv);
}
