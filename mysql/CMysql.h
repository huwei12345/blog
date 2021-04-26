//mysql���ӳ�
//#pragma once
#ifndef CMYSQL_DEFINE
#define CMYSQL_DEFINE
#include "mysql.h"
#include"QueryResult.h"
#include<iostream>
#include<string.h>
class CMysql//mysql��
{
private:
	MYSQL* m_Mysql;
	bool m_bInit;
	std::string strHost;
	std::string strUser;
	std::string strPwd;
	std::string strDBName;
	//����
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