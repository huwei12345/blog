#include "MysqlManager.h"
#include "mysqlpool.h"
//除了多一个搜索用户信息的是用账号密码，其他都是增删改查用id搜索
//当增 改的时候需要整个结构体传入，并根据结构体修改数据库
//当删除的时候和查找的时候，删除只需传入几个参数，查找需要返回结构体

//数据库操作（非Get）返回结果
//插入 删除 修改的结果 1 -1 0

std::shared_ptr<CMysql>* mysqlpool;
int *mysqlqueue=NULL;
void mysql_init()
{
    int n=10;
    string host("localhost");
    string user("root");
    string pwd("123456");
    string dbName("blog");
    //CMysqlManager mgr;
    //if (mgr.init(host.c_str(), user.c_str(), pwd.c_str(), dbName.c_str()) == false)
    //    return;    
    mysqlpool = new std::shared_ptr<CMysql>[n];
    mysqlqueue=new int[n];
    for(int i=0;i<n;i++)
    {
        mysqlpool[i]=static_cast<std::shared_ptr<CMysql>>(new CMysql());
        mysqlpool[i]->init(host,user,pwd,dbName);
        mysqlqueue[i]=0;
    }
}

CMysql* get_mysql_handler()
{
    //阻塞获取
    int n=10;
    for(int i=0;i<n;i++)
    {
        if(mysqlqueue[i]==0)
        {
            mysqlqueue[i]=1;
            return mysqlpool[i].get();
        }
    }
    return NULL;
}

Result insert_user(struct User *p)
{
    char* str = new char[100];
	snprintf(str, 100, "insert into users_t(name,address,sex,create_time,fans_num,article_num,account,password) values");
    snprintf(str, 200, "%s('%s','%s','%s','%s',%d,%d,'%s','%s');", str, p->name, p->address
		, p->sex, p->create_time, p->fans_num,p->article_num, p->account, p->password);
	CMysql* m_mysql=get_mysql_handler();
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if (m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    delete[] str;
    return SUCCESS;
}

Result insert_user_rel(struct User_Relation *user_relation)
{
    char* str = new char[100];
	snprintf(str, 100, "insert into users_rel_t(user_id,rel_user_id) values(");
    snprintf(str, 100, "%s(%d,%d);", str, user_relation->user_id, user_relation->rel_user_id);
    CMysql* m_mysql=get_mysql_handler();
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
	if (m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    delete[] str;
    return SUCCESS;
}
Result insert_group(struct Group *group)
{

}
Result insert_article(struct Article *article)
{

}
Result insert_comment(struct Comment *comment)
{

}
Result insert_collect(struct Collect *collect)
{

}
User* query_my_user(char* account,char* password)
{
    
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select user_id,name,address,sex,create_time,fans_num,article_num from users_t where account='%s' and password='%s';",account,password);
    CMysql* mysql=get_mysql_handler();
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    if(res==NULL)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        User* user=new User;
        //只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if (pRow == NULL)
			return NULL;
		user->user_id=pRow[0].getInt32();
		user->name = const_cast<char*>(pRow[1].getString().c_str());;
		printf("naem =%s \n",user->name);
        user->address = const_cast<char*>(pRow[2].getString().c_str());
        printf("naem =%s \n",user->address);
		user->sex = const_cast<char*>(pRow[3].getString().c_str());
		printf("naem =%s \n",user->sex);
        user->create_time = const_cast<char*>(pRow[4].getString().c_str());
		printf("naem =%s \n",user->create_time);
        user->fans_num=pRow[5].getInt32();
        printf("naem =%d \n",user->fans_num);
		user->article_num = pRow[6].getInt32();
        printf("naem =%d \n",user->article_num);
        printf("%d %s %s %s %s %d %d\n",user->user_id,user->name,user->address,user->sex,user->create_time,user->fans_num,user->article_num);
	    res->endQuery();
        delete res;
        return user;
    }
    return NULL;
}
User* query_user(unsigned int user_id)
{
    //根据user_id获取个人信息，非本人信息
}
User_Relation* query_user_rel(unsigned int user_id)
{
    //根据user_id获取个人本人关注的用户的id
}
Group* query_group(unsigned int user_id)
{
    //根据user_id获取个人目录的组信息
}
Article* query_article_title(unsigned int user_id)
{
    //返回名字数组
    //根据user_id获取个人的文章名，用于构建目录，在点击时在读取文章
}
Article* query_article(unsigned int art_id)
{
    //根据文章id获取个人的文章信息
}
Comment* query_comment(unsigned int art_id)
{
    //根据文章id获取评论/回答
}
Collect* query_collect(unsigned int user_id)
{
    //根据用户id获取个人收藏文章id
}
Result modify_user(struct User *user)
{
    char* str = new char[100];
    int flag=1;//用于','的使用
	snprintf(str, 100, "update user_t set");
    if(user->name!=NULL)
        snprintf(str, 100,"%s name=%s", str, user->name);
	if(user->sex!=NULL)
        snprintf(str,100,"%s,sex=%s",str,user->sex);
    if(user->address!=NULL)
        snprintf(str,100,"%s,address=%s",str,user->address);
    if(user->password!=NULL)
        snprintf(str,100,"%s,password=%s",str,user->password);
    CMysql* m_mysql=get_mysql_handler();
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if (m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data modify error" << endl;
		return MODIFY_ERROR;
	}
    delete[] str;
    return SUCCESS;
}
Result modify_user_rel(struct User_Relation *user_relation)
{

}
Result modify_group(struct Group *group)
{

}
Result modify_article(struct Article *article)
{

}
Result modify_comment(struct Comment *comment)
{

}
Result modify_collect(struct Collect *collect)
{

}
Result delete_user(int user_id)
{
    char* str = new char[100];
    int flag=1;//用于','的使用
	snprintf(str, 100, "delete from user_t where");
    snprintf(str,100,"%s user_id=%d",str,user_id);
    CMysql* m_mysql=get_mysql_handler();
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if (m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data modify error" << endl;
		return MODIFY_ERROR;
	}
    delete[] str;
    return SUCCESS;
}
Result delete_user_rel(int user_id,int user_rel_id)
{
    //根据用户id和关注者id，删除关注关系
}
Result delete_all_user_rel(int user_id)
{
    //根据用户id删除其所有关注
}
Result delete_group(int user_id,int group_id)
{
    //删除某个分组
}
Result delete_all_group(int user_id)
{
    //删除个人所有分组
}
Result delete_article(int art_id)
{
    //删除谋篇文章
}
Result delete_all_article(int user_id)
{
    //删除个人所有文章
}
Result delete_comment(int art_id,int comment_id)
{
    //删除某条评论
}
Result delete_comment_all(int art_id)
{
    //删除该文章所有评论
}
Result delete_collect(int user_id,int art_id)
{
    //删除个人的某个收藏
}
Result delete_collect_all(int user_id)
{
    //删除个人所有收藏
}