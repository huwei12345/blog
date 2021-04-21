#include"CMysql.h"
CMysql::CMysql(void)
{
    //m_Mysql = new MYSQL;
    m_Mysql = NULL;
    m_bInit = false;
}

CMysql::~CMysql(void)
{
    if (m_Mysql != NULL)
    {
        if (m_bInit)
        {
            mysql_close(m_Mysql);
        }

        //delete m_Mysql;
    }
}

bool CMysql::init(const std::string& host, const std::string& user, const std::string& pwd, const std::string& dbname)
{
    //LOGI << "CMysql::Initialize, begin...";

    //ClearStoredResults();
    if (m_bInit)
    {
        mysql_close(m_Mysql);
    }

    m_Mysql = mysql_init(m_Mysql);
    m_Mysql = mysql_real_connect(m_Mysql, host.c_str(), user.c_str(), pwd.c_str(), dbname.c_str(), 0, NULL, 0);

    //ClearStoredResults();

    //LOGI << "mysql info: host=" << host << ", user=" << user << ", password=" << pwd << ", dbname=" << dbname;

    strDBName = dbname;
    strHost = host;
    strUser = user;
    strPwd = pwd;

    if (m_Mysql)
    {
        //LOGI << "m_Mysql address " << (long)m_Mysql;
        //LOGI << "CMysql::Initialize, set names utf8";
        mysql_query(m_Mysql, "set names gbk");
        //mysql_query(m_Mysql, "set names latin1");
        m_bInit = true;
        return true;
    }
    else
    {
        //LOGE << "Could not connect to MySQL database at " << host.c_str()
        //    << ", " << mysql_error(m_Mysql);
        mysql_close(m_Mysql);
        return false;
    }

    //LOGI << "CMysql::Initialize, init failed!";
    return false;
}

//TODO: 这个函数要区分一下空数据集和出错两种情况
QueryResult* CMysql::query(const char* sql)
{
    if (!m_Mysql)
    {
        //LOGI << "CMysql::Query, mysql is disconnected!";
        if (false == init(strHost,strUser,strPwd,strDBName))
        {
            return NULL;
        }
    }

    if (!m_Mysql)
        return 0;

    MYSQL_RES* result = 0;
    uint64_t rowCount = 0;
    uint32_t fieldCount = 0;

    {
        //LOGI << sql;
        int iTempRet = mysql_real_query(m_Mysql, sql, strlen(sql));
        if (iTempRet)
        {
            unsigned int uErrno = mysql_errno(m_Mysql);
            //LOGI << "CMysql::Query, mysql is abnormal, errno : " << uErrno;
            if (CR_SERVER_GONE_ERROR == uErrno)
            {
                //LOGI << "CMysql::Query, mysql is disconnected!";
                if (false == init(strHost, strUser,strPwd,strDBName))
                {
                    return NULL;
                }
                //LOGI << sql;
                iTempRet = mysql_real_query(m_Mysql, sql, strlen(sql));
                if (iTempRet)
                {
                    //LOGE << "SQL: " << sql ;
                    //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                }
            }
            else
            {
                //LOGE << "SQL: " << sql ;
                //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                return NULL;
            }
        }

        //LOGI << "call mysql_store_result";
        result = mysql_store_result(m_Mysql);

        rowCount = mysql_affected_rows(m_Mysql);
        fieldCount = mysql_field_count(m_Mysql);
        // end guarded block
    }

    if (!result)
        return NULL;

    //  if (!rowCount)
    //  {
          //LOGI << "call mysql_free_result";
    //      mysql_free_result(result);
    //      return NULL;
    //  }

    QueryResult* queryResult = new QueryResult(result, rowCount, fieldCount);

    queryResult->nextRow();

    return queryResult;
}

bool CMysql::execute(const char* sql)
{
    if (!m_Mysql)
        return false;

    {
        int iTempRet = mysql_query(m_Mysql, sql);
        if (iTempRet)
        {
            unsigned int uErrno = mysql_errno(m_Mysql);
            //LOGI << "CMysql::Query, mysql is abnormal, errno : " << uErrno;
            if (CR_SERVER_GONE_ERROR == uErrno)
            {
                //LOGI << "CMysql::Query, mysql is disconnected!";
                if (false == init(strHost,strUser,strPwd,strDBName))
                {
                    return false;
                }
                //LOGI << sql;
                iTempRet = mysql_real_query(m_Mysql, sql, strlen(sql));
                if (iTempRet)
                {
                    //LOGE("sql error: %s, sql: %s", mysql_error(m_Mysql), sql);
                    //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                }
            }
            else
            {
                //LOGE << "SQL: " << sql;
                //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                //LOGE("sql error: %s, sql: %s", mysql_error(m_Mysql), sql);
            }
            return false;
        }
    }

    return true;
}

bool CMysql::execute(const char* sql, uint32_t& uAffectedCount, int& nErrno)
{
    if (!m_Mysql)
        return false;

    {
        int iTempRet = mysql_query(m_Mysql, sql);
        if (iTempRet)
        {
            unsigned int uErrno = mysql_errno(m_Mysql);
            //LOGE << "CMysql::Query, mysql is abnormal, errno : " << uErrno;
            if (CR_SERVER_GONE_ERROR == uErrno)
            {
                //LOGE << "CMysql::Query, mysql is disconnected!";
                if (false == init(strHost,strUser,strPwd,strDBName))
                {
                    return false;
                }
                //LOGI << sql;
                iTempRet = mysql_query(m_Mysql, sql);
                nErrno = iTempRet;
                if (iTempRet)
                {
                    //LOGE << "SQL: " << sql;
                    //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                }
            }
            else
            {
                //LOGE << "SQL: " << sql;
                //LOGE << "query ERROR: " << mysql_error(m_Mysql);
            }
            return false;
        }
        uAffectedCount = static_cast<uint32_t>(mysql_affected_rows(m_Mysql));
    }

    return true;
}

void CMysql::clearStoredResults()
{
    if (!m_Mysql)
    {
        return;
    }

    MYSQL_RES* result = NULL;
    while (!mysql_next_result(m_Mysql))
    {
        if ((result = mysql_store_result(m_Mysql)) != NULL)
        {
            mysql_free_result(result);
        }
    }
}

uint32_t CMysql::getInsertID()
{
    return (uint32_t)mysql_insert_id(m_Mysql);
    //函数返回上一步 INSERT 操作产生的 ID
}

//函数转义 SQL 语句中使用的字符串中的特殊字符。 下列字符受影响: 如果成功,则该函数返回被转义的字符串。如果失败,则返回 false。
int32_t CMysql::escapeString(char* szDst, const char* szSrc, uint32_t uSize)
{
    if (m_Mysql == NULL)
    {
        return 0;
    }
    if (szDst == NULL || szSrc == NULL)
    {
        return 0;
    }

    return mysql_real_escape_string(m_Mysql, szDst, szSrc, uSize);
}

MYSQL_RES* CMysql::query(const char* sql,int k)
{
    if (!m_Mysql)
    {
        //LOGI << "CMysql::Query, mysql is disconnected!";
        if (false == init(strHost, strUser, strPwd, strDBName))
        {
            return NULL;
        }
    }

    if (!m_Mysql)
        return 0;

    MYSQL_RES* result = 0;
    uint64_t rowCount = 0;
    uint32_t fieldCount = 0;

    {
        //LOGI << sql;
        int iTempRet = mysql_real_query(m_Mysql, sql, strlen(sql));
        if (iTempRet)
        {
            unsigned int uErrno = mysql_errno(m_Mysql);
            //LOGI << "CMysql::Query, mysql is abnormal, errno : " << uErrno;
            if (CR_SERVER_GONE_ERROR == uErrno)
            {
                //LOGI << "CMysql::Query, mysql is disconnected!";
                if (false == init(strHost, strUser, strPwd, strDBName))
                {
                    return NULL;
                }
                //LOGI << sql;
                iTempRet = mysql_real_query(m_Mysql, sql, strlen(sql));
                if (iTempRet)
                {
                    //LOGE << "SQL: " << sql ;
                    //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                }
            }
            else
            {
                //LOGE << "SQL: " << sql ;
                //LOGE << "query ERROR: " << mysql_error(m_Mysql);
                return NULL;
            }
        }
        else
            result = mysql_use_result(m_Mysql);
        //LOG << "call mysql_store_result";
        return result;
    }
}