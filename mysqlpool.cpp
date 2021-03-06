#include "MysqlManager.h"
#include "mysqlpool.h"
//除了多一个搜索用户信息的是用账号密码，其他都是增删改查用id搜索
//当增 改的时候需要整个结构体传入，并根据结构体修改数据库
//当删除的时候和查找的时候，删除只需传入几个参数，查找需要返回结构体

//数据库操作（非Get）返回结果
//插入 删除 修改的结果 1 -1 0
//pool应该设计为类且是一个单例，在运行开始初始化,运行参数指定pool中的数据库连接数
/*
class MysqlManager
{
    public:
    MysqlManager():mysqlqueue(NULL),
                    mysqlpool(NULL),
                    number(0)
    {}
    void mysql_init();
    CMysql* get_mysql_handler();
    get_number();
    private:
    int number;
    std::shared_ptr<CMysql>* mysqlpool;
    int *mysqlqueue=NULL;

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
    string dbName("blog2");
    //CMysqlManager mgr;
    //if(mgr.init(host.c_str(), user.c_str(), pwd.c_str(), dbName.c_str()) == false)
    //    return;    
    mysqlpool = new std::shared_ptr<CMysql>[n];
    mysqlqueue=new int[n];
    for(int i=0;i<n;i++)
    {
        mysqlpool[i]=static_cast<std::shared_ptr<CMysql>>(new CMysql());
        mysqlpool[i]->init(host,user,pwd,dbName);
        mysqlqueue[i]=0;
        mysqlpool[i]->execute("set names utf8mb4;");
    }
}
CMysql* get_mysql_handler(int *count)
{
    //阻塞获取
    int n=10;
    for(int i=0;i<n;i++)
    {
        if(mysqlqueue[i]==0)
        {
            mysqlqueue[i]=1;
            *count=i;
            return mysqlpool[i].get();
        }
    }
    return NULL;
}

User* query_my_user(char* account,char* password)
{
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人,只有一个，只返回一个结构体
    char* str = new char[200];
    snprintf(str,200,"select user_id,name,address,sex,create_time,fans_num,article_num from users_t where account='%s' and password='%s';",account,password);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        User* user=new User;
        //只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if(pRow == NULL)
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

User* query_user(int user_id)
{
    //根据user_id获取个人信息，非本人信息,只返回一个
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select user_id,name,address,sex,create_time,fans_num,article_num from users_t where user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        User* user=new User;
        //只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if(pRow == NULL)
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
int query_have_user_account(char* account)//不需要返回数组
{
    //根据user_id获取个人信息，非本人信息
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM users_t WHERE account='%s';",account);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return 1;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return 0;
    }
    res->endQuery();
    delete res;
    return 1;
}
User_Relation* query_user_rel(int user_id,int* count)
{
    //要返回数组
    //根据user_id获取个人本人关注的用户的id
    QueryResult* res=NULL;
    *count=0;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[250];
    snprintf(str,200," select users_t.user_id,rel_user_id,users_t.name from users_rel_t,users_t where users_rel_t.rel_user_id=users_t.user_id and users_rel_t.user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        User_Relation* user_rel=new User_Relation[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            user_rel[i].user_id=pRow[0].getInt32();
            user_rel[i].rel_user_id=pRow[1].getInt32();

            int length=pRow[2].getString().length()+1;
            user_rel[i].name=new char[length];
            strcpy(user_rel[i].name,pRow[2].getString().c_str());
            user_rel->name[length-1]='\0';
            if(!res->nextRow())
                break;
            i++;
	    }
		res->endQuery();
        delete res;
        return user_rel;
	}
    return NULL;
}

Group* query_group(int user_id,int* count)
{
    //根据user_id获取个人目录的组信息
        //要返回数组
    //根据user_id获取个人本人关注的用户的id
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select user_id,group_id,group_name,father_group from group_t where user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Group* group=new Group[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            group[i].user_id=pRow[0].getInt32();
            group[i].group_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            group[i].group_name=new char[length];
            strcpy(group[i].group_name,pRow[2].getString().c_str());
            group[i].group_name[length-1]='\0';
            
            group[i].father_group_id=pRow[3].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return group;
    }
    return NULL;
}

Article* query_article_title(int user_id,int* count)
{
    //返回名字数组
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[200];
    snprintf(str,200,"select art_id,user_id,title,upvote_num,create_time,modify_time,group_id from article_t where user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}

Article* query_article(int art_id)//不需要返回数组
{
    //根据文章id获取个人的文章信息
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select art_id,user_id,title,upvote_num,create_time,modify_time,group_id,article,type from article_t where art_id=%d;",art_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        Article* article=new Article;
        //不只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if(pRow == NULL)
			return NULL;
        article->art_id=pRow[0].getInt32();
        article->user_id=pRow[1].getInt32();
        
        int length=pRow[2].getString().length()+1;
        article->title=new char[length];
        strcpy(article->title,pRow[2].getString().c_str());
        article->title[length-1]='\0';
        article->upvote_num=pRow[3].getInt32();
        
        length=pRow[4].getString().length()+1;
        article->create_time=new char[length];
        strcpy(article->create_time,pRow[4].getString().c_str());
        article->create_time[length-1]='\0';
        length=pRow[5].getString().length()+1;
        article->modify_time=new char[length];
        strcpy(article->modify_time,pRow[5].getString().c_str());
        article->modify_time[length-1]='\0';
        article->group_id=pRow[6].getInt32();    
        length=pRow[7].getString().length()+1;
        article->text=new char[length];
        strcpy(article->text,pRow[7].getString().c_str());
        article->text[length-1]='\0';
        article->type=pRow[8].getInt32();

        res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
User* query_user_name(char* name,int* count)
{
    QueryResult* res=NULL;
    char* str = new char[200];
    snprintf(str,200,"select user_id,name,address,sex,create_time,fans_num,article_num from users_t where name like '%%%s%%';",name);
    int sql_index=0;
    printf("sql:%s\n",str);
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        User* user=new User[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            user[i].user_id=pRow[0].getInt32();
            int length=0;
            length=pRow[1].getString().length()+1;
            user[i].name = new char[length];
            strcpy(user[i].name,pRow[1].getString().c_str());
            user[i].name[length-1]='\0';

            length=pRow[2].getString().length()+1;
            user[i].address=new char[length];
            strcpy(user[i].address,pRow[2].getString().c_str());
            user[i].address[length-1]='\0';

            length=pRow[3].getString().length()+1;
            user[i].sex=new char[length];
            strcpy(user[i].sex,pRow[3].getString().c_str());
            user[i].sex[length-1]='\0';

            length=pRow[4].getString().length()+1;
            user[i].create_time=new char[length];
            strcpy(user[i].create_time,pRow[4].getString().c_str());
            user[i].create_time[length-1]='\0';

            user[i].fans_num=pRow[5].getInt32();
            user[i].article_num = pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return user;
    }
    return NULL;
}
Article* query_article_name(char* name,int* count)
{
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[200];
    snprintf(str,200,"select art_id,user_id,title,upvote_num,create_time,modify_time,group_id from article_t where title like '%%%s%%';",name);
    printf("sql:%s\n",str);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
Article* query_article_id(int user_id)
{
    //获取user_id对应的最大aricle_id
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select max(art_id) from article_t where user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        Article* article=new Article;
        //不只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if(pRow == NULL)
			return NULL;
        article->art_id=pRow[0].getInt32();
        res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
Group* query_group_id(int user_id)
{ 
    //获取user_id对应的最大aricle_id
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select max(group_id) from group_t where user_id=%d;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        Group* group=new Group;
        //不只取第一个，应该也只有一个，在插入时保证账号唯一
		Field* pRow = res->fetch();
		if(pRow == NULL)
			return NULL;
        group->group_id=pRow[0].getInt32();
        res->endQuery();
        delete res;
        return group;
    }
    return NULL;
}


Comment* query_comment(int art_id,int* count)
{
    //根据文章id获取评论/回答
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[200];
    snprintf(str,200,"select com_id,art_id,com_user_id,com_text,upvote_num,is_question from comment_t where art_id=%d;",art_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Comment* comment=new Comment[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            comment[i].comment_id=pRow[0].getInt32();
            comment[i].art_id=pRow[1].getInt32();
            comment[i].com_user_id=pRow[2].getInt32();
            int length=pRow[3].getString().length()+1;
            comment[i].text=new char[length];
            strcpy(comment[i].text,pRow[3].getString().c_str());
            comment[i].text[length-1]='\0';
            comment[i].upvote_num=pRow[4].getInt32();
            comment[i].is_question=pRow[5].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return comment;
    }
    return NULL;
}
Collect* query_collect(int user_id,int* count)
{
    //根据用户id获取个人收藏文章id
    QueryResult* res=NULL;
    //根据用户密码，获取个人信息，返回个人信息id等，本人
    char* str = new char[300];
    snprintf(str,300,"select collect_t.user_id,collect_t.collect_art_id,collect_t.collect_num,article_t.title,users_t.user_id from collect_t,article_t,users_t where collect_t.user_id=%d and collect_t.collect_art_id=article_t.art_id and users_t.user_id=article_t.user_id;",user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Collect* collect=new Collect[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            collect[i].user_id=pRow[0].getInt32();
            collect[i].collect_art_id=pRow[1].getInt32();
            collect[i].collect_num=pRow[2].getInt32();
            int length=pRow[3].getString().length()+1;
            collect[i].art_name=new char[length];
            strcpy(collect[i].art_name,pRow[3].getString().c_str());
            collect[i].art_name[length-1]='\0';            
            collect[i].art_user_id=pRow[4].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return collect;
    }
    return NULL;
}

Status query_user_rel_exist(User_Relation* rel)
{
    //查询是否存在关注关系
    QueryResult* res=NULL;
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM users_rel_t WHERE user_id=%d and rel_user_id=%d;",rel->user_id,rel->rel_user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return FAILURE;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return FAILURE;
    }
    res->endQuery();
    delete res;
    return SUCCESS;
}
Status query_user_col_exist(Collect* col)
{
    //查询是否存在收藏关系
    QueryResult* res=NULL;
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM collect_t WHERE user_id=%d and collect_art_id=%d;",col->user_id,col->collect_art_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return FAILURE;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return FAILURE;
    }
    res->endQuery();
    delete res;
    return SUCCESS;

}

Status insert_user(User *p)
{
    printf("insert_user %s\n",p->name);
    char* str = new char[200];
    char* temp=new char[100];
    snprintf(str, 200, "insert into users_t(name,address,sex,create_time,fans_num,article_num,account,password) values(");
	if(p->name!=NULL)
        snprintf(temp,100,"'%s'",p->name);
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }
    str=strcat(str,temp);
    if(p->address!=NULL)
        snprintf(temp,100,",'%s'",p->address);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    if(p->sex!=NULL)
        snprintf(temp,100,",'%s'",p->sex);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    
    if(p->create_time!=NULL)
        snprintf(temp,100,",now()");
    else
        snprintf(temp,100,",now()");
    str=strcat(str,temp);

    if(p->fans_num!=-1)
        snprintf(temp,100,",%d",p->fans_num);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    if(p->article_num!=-1)
        snprintf(temp,100,",%d",p->article_num);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    if(p->account!=NULL)
        snprintf(temp,100,",'%s'",p->account);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    if(p->password!=NULL)
        snprintf(temp,100,",'%s'",p->password);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    snprintf(temp,100,");");
    strcat(str,temp);
    printf("sql=%s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlpool[sql_index]=0;
    delete[] str;
    delete[] temp;
    return SUCCESS;
}

Status insert_user_rel(User_Relation *user_relation)
{
    char* str = new char[100];
    char* temp=new char[20];
    if(user_relation->rel_user_id<=0||user_relation->user_id<=0)
        return INSERT_ERROR;
    snprintf(str, 200, "insert into users_rel_t(user_id,rel_user_id) values(");
    // %d,%d);",user_relation->user_id, user_relation->rel_user_id
    if(user_relation->user_id!=-1)
        snprintf(temp,100,"%d",user_relation->user_id);
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }
    str=strcat(str,temp);
    if(user_relation->rel_user_id!=-1)
        snprintf(temp,100,",%d",user_relation->rel_user_id);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    snprintf(temp,100,");");
    strcat(str,temp);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    printf("str = %s\n",str);
    if(m_mysql==NULL)
    {
        return GET_MYSQL_ERROR;
    }
    if((m_mysql->execute(str))==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status insert_group(Group *group)
{
    char* str = new char[200];
    char* temp=new char[200];
    if(group->group_name==NULL)
        return INSERT_ERROR;

    snprintf(str, 300, "insert into group_t(user_id,group_id,group_name,father_group) values(");
    printf("sql:%s\n",str);
    //insert into group_t(user_id,group_id,group_name,father_group) values(1,(select max(group_id) from group_t as b where b.user_id=1)+1,'new group',0);
    if(group->user_id!=-1)
    {
        snprintf(temp,100,"%d",group->user_id);
        str=strcat(str,temp);
    }
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }
    snprintf(temp,200,",(select ifnull(max(b.group_id),0) from group_t as b where b.user_id=%d)+1",group->user_id);
    str=strcat(str,temp);
    if(group->group_name!=NULL)
        snprintf(temp,100,",'%s'",group->group_name);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    if(group->father_group_id!=-1)
        snprintf(temp,100,",%d",group->father_group_id);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);    
    snprintf(temp,100,");");
    strcat(str,temp);
    printf("str = %s\n",str);
    int sql_index=0;

    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
    {
        return GET_MYSQL_ERROR;
    }
    if((m_mysql->execute(str))==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
Status insert_article(Article *article)
{
    int len=strlen(article->text);
    printf("article len=%d\n",len);
    char* temp=new char[len+100];
    char* str = new char[200+len];
    memset(temp,'\0',len+100);
    memset(str,'\0',len+200);
    snprintf(str, 200, "insert into article_t(user_id,title,upvote_num,create_time,modify_time,group_id,type,article) values(");
    //%d,'%s','%s',%d,'%s','%s',%d);",
    //    article->user_id, article->title,article->text,article->upvote_num,article->create_time,article->modify_time,article->group_id    
    if(article->user_id!=-1)
        snprintf(temp,100,"%d",article->user_id);
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }
    str=strcat(str,temp);
    if(article->title!=NULL)
        snprintf(temp,100,",'%s'",article->title);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);

    if(article->upvote_num!=-1)
        snprintf(temp,100,",%d",article->upvote_num);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    if(article->create_time!=NULL)
        snprintf(temp,200,",now()");
    else
        snprintf(temp,200,",now()");
    str=strcat(str,temp);

    if(article->modify_time!=NULL)
        snprintf(temp,200,",now()");
    else
        snprintf(temp,200,",now()");
    str=strcat(str,temp);
    if(article->group_id!=-1)
        snprintf(temp,100,",%d",article->group_id);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    if(article->type!=-1)
        snprintf(temp,200,",%d",article->type);
    else
        snprintf(temp,200,",0");
    str=strcat(str,temp);

    if(article->text!=NULL)//内存太多，不如写if else(特殊处理，spinrf(temp+strlen(temp),"'%s'",article->text) else NULL)
        snprintf(temp,200+len,",'%s'",article->text);
    else
        snprintf(temp,200,",NULL");
    str=strcat(str,temp);
    
    snprintf(temp,100,");");
    strcat(str,temp);
    printf("str = %s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
    {
        return GET_MYSQL_ERROR;
    }
    if((m_mysql->execute(str))==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    delete[] temp;
    return SUCCESS;
}
Status insert_comment(Comment *comment)
{
    int len=strlen(comment->text);
    char* temp=new char[len+1];
    char* str = new char[200+len];
    snprintf(str, 200+len, "insert into comment_t(art_id,com_user_id,com_text,upvote_num,is_question) values(");
    //%d,%d,%d,'%s',%d,%d);",
    //    comment->comment_id,comment->art_id,comment->com_user_id,comment->text,comment->upvote_num,comment->is_question    
    if(comment->art_id!=-1)
    {
        snprintf(temp,200,"%d",comment->art_id);
        str=strcat(str,temp);
    }
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }

    if(comment->com_user_id!=-1)
        snprintf(temp,100,",%d",comment->com_user_id);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    if(comment->text!=NULL) 
        snprintf(temp,100,",'%s'",comment->text);
    else
        snprintf(temp,100,",NULL");
    str=strcat(str,temp);
    if(comment->upvote_num!=-1)
        snprintf(temp,100,",%d",comment->upvote_num);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);

    if(comment->is_question!=-1)
        snprintf(temp,100,",%d",comment->is_question);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    snprintf(temp,100,");");
    strcat(str,temp);
    printf("str = %s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
    {
        return GET_MYSQL_ERROR;
    }
    if((m_mysql->execute(str))==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
Status insert_collect(Collect *collect)
{
    char* str = new char[200];
    char* temp=new char[40];
    snprintf(str, 200, "insert into collect_t(user_id,collect_art_id,collect_num) values(");
    
    if(collect->user_id!=-1)
        snprintf(temp,200,"%d",collect->user_id);
    else
    {
        delete[] str;
        delete[] temp;
        return INSERT_ERROR;
    }
    str=strcat(str,temp);
    if(collect->collect_art_id!=-1)
        snprintf(temp,200,",%d",collect->collect_art_id);
    else
        snprintf(temp,200,",0");
    str=strcat(str,temp);

    if(collect->collect_num!=-1)
        snprintf(temp,100,",%d",collect->collect_num);
    else
        snprintf(temp,100,",0");
    str=strcat(str,temp);
    snprintf(temp,100,");");
    strcat(str,temp);
    printf("str = %s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
    {
        return GET_MYSQL_ERROR;
    }
    if((m_mysql->execute(str))==false)//插入数据,已经有了
	{
		cout << "data insert error" << endl;
		return INSERT_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}


//修改的sql语句组织较繁琐，只支持修改user、group、article，其他只支持增删。
Status modify_user(User *user)
{
    if(user->user_id==-1)
        return MODIFY_ERROR;
    //int flag=0;
    char* str=new char[200];
	snprintf(str,100,"update users_t set");
    if(user==NULL)
        return MODIFY_ERROR;
    if(user->name!=NULL)
    {
        //把名字传来吧，“,”的逻辑太多了
        snprintf(str+strlen(str),100," name='%s'",user->name);
        //flag=0;
    }
    if(user->sex!=NULL)
        snprintf(str+strlen(str),100,",sex='%s'",user->sex);
    if(user->address!=NULL)
        snprintf(str+strlen(str),100,",address='%s'",user->address);
    snprintf(str+strlen(str),100," where user_id=%d;",user->user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    printf("sql:%s\n",str);
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "user modify error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
//user_rel不需要修改，只有增删
//group可能修改父节点，名字
Status modify_group(Group *group)
{
    char* str = new char[200];
	snprintf(str, 100, "update group_t set");
    if(group==NULL)
        return MODIFY_ERROR;
    snprintf(str+strlen(str),200," father_group=%d ",group->father_group_id);
    snprintf(str+strlen(str),200," where group_id=%d and user_id=%d;",group->group_id,group->user_id);
    printf("sql:%s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "group modify error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;

}
Status modify_article(Article *article)
{
    int len=strlen(article->text);
    char* str = new char[200+len];
	snprintf(str, 100, "update article_t set");
    if(article==NULL)
        return MODIFY_ERROR;
    if(article->title!=NULL)
    {
        //把名字传来吧，“,”的逻辑太多了
        snprintf(str+strlen(str),200," title='%s'",article->title);
        //flag=0;
    }
    if(article->type!=-1)
        snprintf(str+strlen(str),200,",type=%d",article->type);
    if(article->text!=NULL)
        snprintf(str+strlen(str),200+len,",article='%s'",article->text);
    snprintf(str+strlen(str),200+len," where art_id=%d and user_id=%d;",article->art_id,article->user_id);
    printf("sql:%s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "article modify error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status modify_article_group(int art_id,int group_id)
{
    char* str = new char[200];
	snprintf(str, 100, "update article_t set");
    if(art_id==-1)
        return MODIFY_ERROR;
    snprintf(str+strlen(str),200," group_id=%d ",group_id);
    snprintf(str+strlen(str),200," where art_id=%d;",art_id);
    printf("sql:%s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "article modify error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
//comment不需要修改，只需要删除和增加

//收藏不需要修改，只支持增删,也许以后有树形结构，再添加修改功能


//删除用户，删除可能有依赖，在删除前，将其依赖删除。可设计一个图，广度优先。
Status delete_user(int user_id)
{
    char* str = new char[100];
    
    snprintf(str,100,"delete from user_t where user_id=%d",user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "data modify error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_user_rel(int user_id,int user_rel_id)
{
    //根据用户id和关注者id，删除关注关系
    char* str = new char[100];
    
    snprintf(str,100,"delete from users_rel_t where user_id=%d and rel_user_id=%d",user_id,user_rel_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "user_rel data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_all_user_rel(int user_id)
{
    //根据用户id删除其所有关注
    char* str = new char[100];
    
    snprintf(str,100,"delete from user_rel where user_id=%d",user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "user_rel data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
Status query_have_article_in(int group_id,int user_id)
{
    QueryResult* res=NULL;
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM article_t WHERE group_id=%d and user_id=%d;",group_id,user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return FAILURE;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    printf("group_id has %lu article\n",res->getRowCount());
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return SUCCESS;
    }
    res->endQuery();
    delete res;
    return FAILURE;
}
Status query_have_group_in(int user_id,int group_id)
{
    QueryResult* res=NULL;
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM group_t WHERE father_group=%d and user_id=%d;",group_id,user_id);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return FAILURE;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    printf("group_id has %lu son group\n",res->getRowCount());
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return SUCCESS;
    }
    res->endQuery();
    delete res;
    return FAILURE;
}
Status delete_group(int user_id,int group_id)
{
    //删除某个分组
    char* str = new char[100];
    snprintf(str,100,"delete from group_t where user_id=%d and group_id=%d",user_id,group_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "group data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
Status delete_all_group(int user_id)
{
    //删除个人所有分组
    char* str = new char[100];
    
    snprintf(str,100,"delete from group_t where user_id=%d",user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "group data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_article(int art_id)
{
    if(art_id!=-1)
    {
        //删除谋篇文章
        char* str = new char[100];
        snprintf(str,100,"delete from article_t where art_id=%d",art_id);
        int sql_index=0;
        CMysql* m_mysql=get_mysql_handler(&sql_index);
        if(m_mysql==NULL)
            return GET_MYSQL_ERROR;
        if(m_mysql->execute(str)==false)//插入数据,已经有了
        {
            cout << "article data delete error" << endl;
            return MODIFY_ERROR;
        }
        mysqlqueue[sql_index]=0;
        delete[] str;
        return SUCCESS;
    }
    else
        return SUCCESS;
}

Status delete_all_article(int user_id)
{
    //删除个人所有文章
    char* str = new char[100];
    snprintf(str,100,"delete from article_t where user_id=%d",user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "article data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_comment(int comment_id)
{
    //删除某条评论
    char* str = new char[100];
    snprintf(str,100,"delete from comment_t where com_id=%d",comment_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "comment data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_comment_all(int art_id)
{
    //删除该文章所有评论
    char* str = new char[100];
    snprintf(str,100,"delete from comment_t where art_id=%d",art_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "comment data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_collect(int user_id,int art_id)
{
    //删除个人的某个收藏
    char* str = new char[100];
    snprintf(str,100,"delete from collect_t where user_id=%d and collect_art_id=%d;",user_id,art_id);
    int sql_index=0;
    printf("sql:%s\n",str);
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "collect data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status delete_collect_all(int user_id)
{
    //删除个人所有收藏
    char* str = new char[100];
    
    snprintf(str,100,"delete from collect_t where user_id=%d",user_id);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "collect data delete error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}

Status add_art_upvote(int art_id)
{
    char* str = new char[200];
	snprintf(str, 100, "update article_t set");
    if(art_id==-1)
        return MODIFY_ERROR;
    snprintf(str+strlen(str),200," upvote_num=upvote_num+1 where art_id=%d;",art_id);
    printf("sql:%s\n",str);
    int sql_index=0;
    CMysql* m_mysql=get_mysql_handler(&sql_index);
    if(m_mysql==NULL)
        return GET_MYSQL_ERROR;
    if(m_mysql->execute(str)==false)//插入数据,已经有了
	{
		cout << "upvote article error" << endl;
		return MODIFY_ERROR;
	}
    mysqlqueue[sql_index]=0;
    delete[] str;
    return SUCCESS;
}
 
Article* query_article_bytype(int type,int *count)
{
    //返回名字数组
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[200];
    snprintf(str,200,"select art_id,user_id,title,upvote_num,create_time,modify_time,group_id,type from article_t where type=%d order by upvote_num desc limit 5;",type);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            article[i].type=pRow[7].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
Article* query_article_bynow(int *count)
{
//返回名字数组
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[300];
    snprintf(str,300,"select art_id,user_id,title,upvote_num,create_time,modify_time,group_id,type from article_t where to_days(now())-to_days(create_time)<=1 order by create_time desc limit 5;");
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
Status query_have_group(int group,int user)
{
    QueryResult* res=NULL;
    char* str=new char[200];
    snprintf(str,200,"SELECT 1 FROM group_t WHERE group_id=%d and user_id=%d;",group,user);
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return FAILURE;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    if(res->getRowCount()==0)
    {
        res->endQuery();
        delete res;
        return FAILURE;
    }
    res->endQuery();
    delete res;
    return SUCCESS;
}
Article* query_article_bymonth(int *count)
{
//返回名字数组
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[300];
    snprintf(str,300,"SELECT art_id,user_id,title,upvote_num,create_time,modify_time,group_id,type from article_t WHERE DATE_SUB(CURDATE(),INTERVAL 1 MONTH) <= DATE(create_time) order by upvote_num desc limit 5;");
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
Article* query_article_byweek(int *count)
{
//返回名字数组
    //根据user_id获取个人的文章名，除了article不查其他都查。用于构建目录，在点击时在读取文章
    QueryResult* res=NULL;
    char* str = new char[300];
    snprintf(str,300,"SELECT art_id,user_id,title,upvote_num,create_time,modify_time,group_id,type from article_t WHERE create_time BETWEEN CURRENT_DATE() -7 AND SYSDATE() order by upvote_num desc limit 5;");
    int sql_index=0;
    CMysql* mysql=get_mysql_handler(&sql_index);
    if(mysql==NULL)
        return NULL;
    res=mysql->query(str);
    mysqlqueue[sql_index]=0;
    delete[] str;
    unsigned long int row_count=res->getRowCount();
    if(res==NULL||row_count==0)//不太对，应该检查是否mysql内容返回为空
        return NULL;
    else
    {
        *count=row_count;
        unsigned long int i=0;
        Article* article=new Article[res->getRowCount()];
        while (i<row_count)
	    {
        	Field* pRow = res->fetch();
            if(pRow == NULL)
                return NULL;
            article[i].art_id=pRow[0].getInt32();
            article[i].user_id=pRow[1].getInt32();
            
            int length=pRow[2].getString().length()+1;
            article[i].title=new char[length];
            strcpy(article[i].title,pRow[2].getString().c_str());
            article[i].title[length-1]='\0';
            article[i].upvote_num=pRow[3].getInt32();
            
            length=pRow[4].getString().length()+1;
            article[i].create_time=new char[length];
            strcpy(article[i].create_time,pRow[4].getString().c_str());
            article[i].create_time[length-1]='\0';
            length=pRow[5].getString().length()+1;
            article[i].modify_time=new char[length];
            strcpy(article[i].modify_time,pRow[5].getString().c_str());
            article[i].modify_time[length-1]='\0';

            article[i].group_id=pRow[6].getInt32();
            if(!res->nextRow())
                break;
            i++;
	    }
	    res->endQuery();
        delete res;
        return article;
    }
    return NULL;
}
//insert里的sprinf是否使用str+strlen(str)作为第一个参数更好，例如modify中,否则插入文章时浪费太多内存
//再之后可以将C重构为C++，再有时间可以考虑event类型、后端日志，前端界面其他功能。