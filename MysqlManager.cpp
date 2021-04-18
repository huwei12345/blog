#include "MysqlManager.h"
#include <sstream>
//#include "MysqlThrdMgr.h"

CMysqlManager::CMysqlManager(void)
{
	//TODO: m_strCharactSet可以放在初始化列表中初始化
    m_strCharactSet = "utf8";
	// 初始化表 
	// 1. t_user 
	STableInfo info;
	//name type desc?
    info.m_strName = "roadinfo";//表名;
    info.m_mapField["LinkID"] = { "LinkID", "INT NOT NULL  COMMENT 'LinkID'", "INT" };
	info.m_mapField["record_size"] = { "record_size", " SMALLINT UNSIGNED NOT NULL COMMENT 'recore length'", "SMALLINT" };
	info.m_mapField["roadsize"] = { "roadsize", " SMALLINT UNSIGNED NOT NULL  COMMENT 'roadsize'", "SMALLINT" };
    info.m_mapField["hasflag"] = { "hasflag", " TINYINT UNSIGNED NOT NULL  COMMENT 'hasflag'", "TINYINT" };
    info.m_mapField["forkroad"] = { "forkroad", " SMALLINT UNSIGNED NOT NULL  COMMENT 'forkroad'", "SMALLINT" };
    info.m_mapField["classnum"] = { "classnum", " SMALLINT UNSIGNED NOT NULL  COMMENT 'classnum'", "SMALLINT" };
    info.m_mapField["information"] = { "information", " INT UNSIGNED NOT NULL  COMMENT 'information'", "INT" };
    info.m_mapField["name"] = { "name", "varchar(40) character set gbk collate gbk_chinese_ci NOT NULL COMMENT 'name'", "nvarchar(40)" };
  
    info.m_strKeyString = "PRIMARY KEY (LinkID), INDEX LinkID (LinkID)";
    m_vecTableInfo.push_back(info);//只有一个表
}

bool CMysqlManager::init(const char* host, const char* user, const char* pwd, const char* dbname)
{
	m_strHost = host;
	m_strUser = user;
	//数据库密码可能为空
	if (pwd != NULL)
		m_strPassword = pwd;
	m_strDataBase = dbname;

	//注意：检查数据库是否存在时，需要将数据库名称设置为空
	m_poConn.reset(new CMysql());
	if (!m_poConn->init(m_strHost, m_strUser, m_strPassword, ""))
	{
		//LOG_FATAL << "CMysqlManager::Init failed, please check params(" << m_strHost << ", " << m_strUser << ", " << m_strPassword << ")";
		return false;
	}

	////////////////////// 1. 检查库是否存在 /////////////////////////
	if (!isDBExist())
	{
		if (!createDB())
		{
			return false;
		}
	}

	//再次确定是否可以连接上数据库
	m_poConn.reset(new CMysql());
	if (!m_poConn->init(m_strHost, m_strUser, m_strPassword, m_strDataBase))
	{
		//LOG_FATAL << "CMysqlManager::Init failed, please check params(" << m_strHost << ", " << m_strUser
		//	<< ", " << m_strPassword << ", " << m_strDataBase << ")";
		return false;
	}

	////////////////////// 2. 检查库中表是否正确 /////////////////////////
	for (size_t i = 0; i < m_vecTableInfo.size(); i++)
	{
		STableInfo table = m_vecTableInfo[i];
		if (!checkTable(table))
		{
			//LOG_FATAL << "CMysqlManager::Init, table check failed : " << table.m_strName;
			return false;
		}
	}
	////////////////////// 2. 检查库中表是否正确 /////////////////////////

	m_poConn.reset();//析构了CMysql
	return true;
}

bool CMysqlManager::isDBExist()
{
	if (NULL == m_poConn)
	{
		return false;
	}

	QueryResult* pResult = m_poConn->query("show databases");
	if (NULL == pResult)
	{
		//LOGI << "CMysqlManager::_IsDBExist, no database(" << m_strDataBase << ")";
		return false;
	}

	Field* pRow = pResult->fetch();
	while (pRow != NULL)
	{
		std::string name = pRow[0].getString();
		if (name == m_strDataBase)
		{
			//LOGI << "CMysqlManager::_IsDBExist, find database(" << m_strDataBase << ")";
			pResult->endQuery();
			return true;
		}

		if (pResult->nextRow() == false)
		{
			break;
		}
		pRow = pResult->fetch();
	}

	//LOGI << "CMysqlManager::_IsDBExist, no database(" << m_strDataBase << ")";
	pResult->endQuery();

	delete pResult;
	return false;
}
bool CMysqlManager::createDB()
{
	if (NULL == m_poConn)
	{
		return false;
	}

	uint32_t uAffectedCount = 0;
	int nErrno = 0;

	std::stringstream ss;
	ss << "create database " << m_strDataBase;
	if (m_poConn->execute(ss.str().c_str(), uAffectedCount, nErrno))
	{
		if (uAffectedCount == 1)
		{
			//LOGI << "CMysqlManager::_CreateDB, create database " <<
			//	m_strDataBase << " success";
			return true;
		}
	}
	else
	{
		//LOGE << "CMysqlManager::_CreateDB, create database " << m_strDataBase << " failed("
		//	<< nErrno << ")";
		return false;
	}
	return false;
}

bool CMysqlManager::checkTable(const STableInfo& table)
{
	if (NULL == m_poConn)
	{
		return false;
	}

	if (table.m_strName.find_first_not_of("\t\r\n ") == std::string::npos)
	{
		//LOGW << "CMysqlManager::_CheckTable, tale info not valid";
		return true;
	}

	std::stringstream ss;
	ss << "desc " << table.m_strName;
	QueryResult* pResult = m_poConn->query(ss.str());
	if (NULL == pResult)
	{
		//LOGI << "CMysqlManager::_CheckTable, no table" << table.m_strName << ", begin create.....";
		if (createTable(table))//没有匹配表
		{
			//LOGI << "CMysqlManager::_CheckTable, " << table.m_strName << ", end create.....";
			return true;
		}
		return false;
	}
	else // 检查字段是否匹配， 暂时只检查是否存在， 还需进一步看类型是否需要修改 
	{
		std::map<std::string, std::string> mapOldTable;
		Field* pRow = pResult->fetch();
		while (pRow != NULL)
		{
			std::string name = pRow[0].getString();
			std::string type = pRow[1].getString();
			mapOldTable[name] = type;

			if (pResult->nextRow() == false)
			{
				break;
			}
			pRow = pResult->fetch();
		}

		pResult->endQuery();
		delete pResult;

		for (std::map<std::string, STableField>::const_iterator it = table.m_mapField.begin();
			it != table.m_mapField.end(); ++it)
		{
			STableField field = it->second;
			if (mapOldTable.find(field.m_strName) == mapOldTable.end())
			{
				std::stringstream ss;
				ss << "alter table " << table.m_strName << " add column "
					<< field.m_strName << " " << field.m_strType;

				std::string sql = ss.str();
				if (m_poConn->execute(sql.c_str()))
				{
					//LOGI << sql;
					continue;
				}
				else
				{
					//LOGE << "CMysqlManager::_CheckTable failed : " << sql;
					return false;
				}
			}
		}
	}

	return true;
}

bool CMysqlManager::createTable(const STableInfo& table)
{
	if (table.m_mapField.size() == 0)
	{
		//LOGE << "CMysqlManager::_CreateTable, table info not valid, " << table.m_strName;
		return false;
	}

	std::stringstream ss;
	ss << "CREATE TABLE IF NOT EXISTS " << table.m_strName << " (";

	for (std::map<std::string, STableField>::const_iterator it = table.m_mapField.begin();
		it != table.m_mapField.end(); ++it)
	{
		if (it != table.m_mapField.begin())
		{
			ss << ", ";
		}

		STableField field = it->second;
		ss << field.m_strName << " " << field.m_strType;
	}

	if (table.m_strKeyString != "")
	{
		ss << ", " << table.m_strKeyString;
	}

	ss << ")default charset = utf8, ENGINE = InnoDB;";
	//ss语句建表
	if (m_poConn->execute(ss.str().c_str()))
	{
		return true;
	}

	//LOGE("Create table error, sql: %s", ss.str().c_str());
	return false;
}