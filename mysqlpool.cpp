#include "MysqlManager.h"
#include "mysqlpool.h"
//除了多一个搜索用户信息的是用账号密码，其他都是增删改查用id搜索
//当增 改的时候需要整个结构体传入，并根据结构体修改数据库
//当删除的时候和查找的时候，删除只需传入几个参数，查找需要返回结构体

//数据库操作（非Get）返回结果
//插入 删除 修改的结果 1 -1 0
//pool应该设计为类且是一个单例，在运行开始初始化,运行参数指定pool中的数据库连接数
/*
class Mysqlpool
{
    Mysqlpool(int n):n=n
    {
        for(n)
            init;
    }
    ~Mysqlpool()
    {
        delete mysqlpool;
    }
public:
    get_num();
    CMysql* get_mysql_handler();
    
private:
    int n;
    std::shared_ptr<CMysql>* mysqlpool;    
};

*/
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

SQL_Rusult insert_user(struct User *p)
{
    char* str = new char[100];
    snprintf(str, 200, "insert into users_t(name,address,sex,create_time,fans_num,article_num,account,password) values('%s','%s','%s','%s',%d,%d,'%s','%s');",p->name, p->address
		, p->sex, p->create_time, p->fans_num,p->article_num, p->account, p->password);
	printf("str = %s\n",str);
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

SQL_Rusult insert_user_rel(struct User_Relation *user_relation)
{
    char* str = new char[100];
    if(user_relation->rel_user_id<=0||user_relation->user_id<=0)
        return INSERT_ERROR;
    snprintf(str, 200, "insert into users_rel_t(user_id,rel_user_id) values(%d,%d);",user_relation->user_id, user_relation->rel_user_id);
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
SQL_Rusult insert_group(struct Group *group)
{
    char* str = new char[100];
    if(group->group_name==NULL)
        return INSERT_ERROR;
    snprintf(str, 200, "insert into group_t(user_id,group_id,group_name,father_group) values(%d,%d,'%s',%d);",group->user_id, group->group_id,group->group_name,group->father_group_id);
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
SQL_Rusult insert_article(struct Article *article)
{
    int len=strlen(article->text);
    char* str = new char[200+len];
    snprintf(str, 200+len, "insert into article_t(user_id,title,article,upvote_num,create_time,modify_time,group_id) values(%d,'%s','%s',%d,'%s','%s',%d);",
        article->user_id, article->title,article->text,article->upvote_num,article->create_time,article->modify_time,article->group_id);
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
SQL_Rusult insert_comment(struct Comment *comment)
{
    int len=strlen(comment->text);
    char* str = new char[200+len];
    snprintf(str, 200+len, "insert into comment_t(com_id,art_id,com_user_id,com_text,upvote_num,is_question) values(%d,%d,%d,'%s',%d,%d);",
        comment->comment_id,comment->art_id,comment->com_user_id,comment->text,comment->upvote_num,comment->is_question);
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
SQL_Rusult insert_collect(struct Collect *collect)
{
    char* str = new char[200];
    snprintf(str, 200, "insert into collect_t(user_id,collect_art_id,collect_num) values(%d,%d,%d);",
        collect->user_id,collect->collect_art_id,collect->collect_num);
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

//查出来的某些字符串不太好显示，需要调试。这部分要写一个星期，顺便加上json和协议。
//下个星期改前端和后端网络库
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
        int length=0;
		user->user_id=pRow[0].getInt32();
        
		length=pRow[1].getString().length()+1;
        user->name = new char[length];
        strcpy(user->name,pRow[1].getString().c_str());
        user->name[length-1]='\0';

        length=pRow[2].getString().length()+1;
        user->address=new char[length];
        strcpy(user->address,pRow[2].getString().c_str());
        user->address[length-1]='\0';

        length=pRow[3].getString().length()+1;
        user->sex=new char[length];
        strcpy(user->sex,pRow[3].getString().c_str());
        user->sex[length-1]='\0';

        length=pRow[4].getString().length()+1;
        user->create_time=new char[length];
        strcpy(user->create_time,pRow[4].getString().c_str());
        user->create_time[length-1]='\0';

        user->fans_num=pRow[5].getInt32();
		user->article_num = pRow[6].getInt32();

	    res->endQuery();
        delete res;
        return user;
    }
    return NULL;
}
User* query_user(unsigned int user_id)
{
    //根据user_id获取个人信息，非本人信息
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select user_id,name,address,sex,create_time,fans_num,article_num from users_t where user_id=%d;",user_id);
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
        int length=0;
		user->user_id=pRow[0].getInt32();
        
		length=pRow[1].getString().length()+1;
        user->name = new char[length];
        strcpy(user->name,pRow[1].getString().c_str());
        user->name[length-1]='\0';

        length=pRow[2].getString().length()+1;
        user->address=new char[length];
        strcpy(user->address,pRow[2].getString().c_str());
        user->address[length-1]='\0';

        length=pRow[3].getString().length()+1;
        user->sex=new char[length];
        strcpy(user->sex,pRow[3].getString().c_str());
        user->sex[length-1]='\0';

        length=pRow[4].getString().length()+1;
        user->create_time=new char[length];
        strcpy(user->create_time,pRow[4].getString().c_str());
        user->create_time[length-1]='\0';

        user->fans_num=pRow[5].getInt32();
		user->article_num = pRow[6].getInt32();

	    res->endQuery();
        delete res;
        return user;
    }
    return NULL;
}
User_Relation* query_user_rel(unsigned int user_id)
{
    //要返回数组
    //根据user_id获取个人本人关注的用户的id
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select user_id，rel_user_id from users_rel_t where user_id=%d;",user_id);
    CMysql* mysql=get_mysql_handler();
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    if(res==NULL)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        User_Relation* user_rel=new User_Relation;
        //不只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if (pRow == NULL)
			return NULL;
        int length=0;
		user_rel->user_id=pRow[0].getInt32();
        
        user_rel->rel_user_id=pRow[1].getInt32();
	    res->endQuery();
        delete res;
        return user_rel;
    }
    return NULL;
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
SQL_Rusult modify_user(struct User *user)
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
SQL_Rusult modify_user_rel(struct User_Relation *user_relation)
{

}
SQL_Rusult modify_group(struct Group *group)
{

}
SQL_Rusult modify_article(struct Article *article)
{

}
SQL_Rusult modify_comment(struct Comment *comment)
{

}
SQL_Rusult modify_collect(struct Collect *collect)
{
    
}
SQL_Rusult delete_user(int user_id)
{
    char* str = new char[100];
    int flag=1;//用于','的使用
    snprintf(str,100,"delete from user_t where user_id=%d",user_id);
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
SQL_Rusult delete_user_rel(int user_id,int user_rel_id)
{
    //根据用户id和关注者id，删除关注关系
}
SQL_Rusult delete_all_user_rel(int user_id)
{
    //根据用户id删除其所有关注
}
SQL_Rusult delete_group(int user_id,int group_id)
{
    //删除某个分组
}
SQL_Rusult delete_all_group(int user_id)
{
    //删除个人所有分组
}
SQL_Rusult delete_article(int art_id)
{
    //删除谋篇文章
}
SQL_Rusult delete_all_article(int user_id)
{
    //删除个人所有文章
}
SQL_Rusult delete_comment(int art_id,int comment_id)
{
    //删除某条评论
}
SQL_Rusult delete_comment_all(int art_id)
{
    //删除该文章所有评论
}
SQL_Rusult delete_collect(int user_id,int art_id)
{
    //删除个人的某个收藏
}
SQL_Rusult delete_collect_all(int user_id)
{
    //删除个人所有收藏
}