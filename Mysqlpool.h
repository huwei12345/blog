#pragma once
#include"elemap.h"
void mysql(Record& record)//用于版本1，将Record写入到mysql。?
{
	string host("localhost");
	string user("root");
	string pwd("123456");
	string dbName("roadinfo");
	CMysqlManager mgr;//mgr中包含CMysql,CMysql不可见，此类仅用于一开始初始化数据库，初始化后会关闭连接
	if (mgr.init(host.c_str(), user.c_str(), pwd.c_str(), dbName.c_str()) == false)//建表
		return;
	char* str = new char[1024*1024];
	//若要查询需要重新建立连接
	std::shared_ptr<CMysql> cmysql=static_cast<std::shared_ptr<CMysql>>(new CMysql());
	cmysql->init(host,user,pwd,dbName);
	cmysql->execute("delete from roadinfo;");

	sprintf_s(str, 150, "insert into roadinfo(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize) values");
	//(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize)
	int size=record.size();
	clock_t start=clock();
	for (int i = 0; i < size; ++i)
	{
		node* p = record[i];
		//格式化一条数据，并加入sql语句
		//sprintf_s(str, 200,"insert into roadinfo values( %d,%d,%d,%d,%d,%d,%d,%s);", record[i]->LinkID,record[i]->record_size
		//	,record[i]->roadsize,record[i]->hasflag,record[i]->forkroad,record[i]->classnum,record[i]->information,record[i]->name);
		
	// record_size Linkid    roadsize   hasflag  forkroad   classnum       roadname
		if ((i>0&&i % 1000 == 0)||i==size-1)
		{
			sprintf_s(str, 1024 * 1024, "%s(%d,%d,%d,%d,%d,'%s',%d,%d);", str, p->LinkID, p->classnum
				, p->forkroad, p->hasflag, p->information, p->name, p->record_size, p->roadsize);
			if (cmysql->execute(str) == false)//插入数据,已经有了
			{
				cout << "insert error" << endl;
				break;
			}
			sprintf_s(str, 150, "insert into roadinfo(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize) values");
		}
		else
			sprintf_s(str, 1024 * 1024, "%s(%d,%d,%d,%d,%d,%s,%d,%d),", str, p->LinkID, p->classnum
				, p->forkroad, p->hasflag, p->information, p->name, p->record_size, p->roadsize);

	}
	clock_t end = clock();
	cout << "cost " << ((double)end - (double)start) / CLOCKS_PER_SEC << " sec" << endl;
	cmysql.reset();
	delete[] str;
	//mysql.~CMysql();
}