//mysql连接池
//#pragma once
#ifndef CMYSQL_DEFINE
#define CMYSQL_DEFINE
#include <winsock.h>//注意这两个头文件的次序不能放错
#include "mysql.h"
#include"QueryResult.h"
#include<iostream>
#pragma comment(lib,"libmysql.lib")
class CMysql//mysql类
{
private:
	MYSQL* m_Mysql;
	bool m_bInit;
	std::string strHost;
	std::string strUser;
	std::string strPwd;
	std::string strDBName;
	//连接
public:
	
	CMysql(void);
	~CMysql(void);
	
	bool init(const std::string& host,const std::string& user, const std::string& pwd, const std::string& dbname);
	
	QueryResult* query(const char* sql);
	MYSQL_RES* query(const char* sql,int k);
	QueryResult* query(const std::string& sql)
	{
		return query(sql.c_str());
	}
	//QueryResult* pquery(const char* format, ...);
	bool execute(const char* sql);
	bool execute(const char* sql, uint32_t& uAffectedCount, int& nErrno);
	//bool pexecute(const char* format, ...);


	uint32_t getInsertID();

	void clearStoredResults();

	int32_t escapeString(char* szDst, const char* szSrc, uint32_t uSize);
};

#endif