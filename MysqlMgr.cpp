#include"elemap.h"
//这些函数许多都需要界面，来输入或者选择选项
void MysqlManager::mysqlsort()
{
	clock_t start=0,end=0;
	double tm = 0;
	char c;
	int flag = 0;
	QueryResult* res=NULL;
	while (1)
	{
		cout << "				select sort by what?   " << endl;
		cout << "				1.LinkID              |" << endl;
		cout << "				2.LinkID(desc)        |" << endl;
		cout << "				3.forkroad            |" << endl;
		cout << "				4.classnum            |" << endl;
		cout << "				5.name                |" << endl;
		cout << "				0.quit" << endl;
		cin >> c;
		char b;
		while ((b = getchar()) != '\n' && b != EOF);//清输入缓冲区
		if (c >= '0' && c <= '9')
		{
			c -= '0';
			start = clock();
			switch (c)
			{
			case 1:
			{
				start = clock();
				res = m_mysql->query("select * from roadinfo order by LinkID;");
				break;
			}
			case 2:
			{
				start = clock();
				res = m_mysql->query("select * from roadinfo order by LinkID desc;");
				break;
			}
			case 3:
			{
				start = clock();
				res = m_mysql->query("select * from roadinfo order by forkroad;");
				break;
			}
			case 4:
			{
				start = clock();
				res = m_mysql->query("select * from roadinfo order by classnum;");
				break;
			}
			case 5:
			{
				start = clock();
				res = m_mysql->query("select * from roadinfo order by name;");
				break;
			}
			case 0:
			{
				flag = 1;
				break;
			}
			default:
			{
				flag = 1;
				break;
			}
			}
		}
		if (flag == 1)
			break;
		end = clock();
		writefile(res, "./sort_record");
		tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
		//showall(record);
		cout << "sort success" << endl;
		cout << "sort cost " << tm << " ms" << endl;
	}
}

void MysqlManager::mysqlsearch()
{
	//按sql语句查询，按字段查询
	clock_t start=0, end=0;
	double tm;
	char b;
	char* value = new char[200];
	char* query = new char[250];
	QueryResult* res=NULL;
	MYSQL_RES* result = NULL;
	int flag = 0;
	while (1)
	{
		strcpy_s(query, 50, "select * from roadinfo where ");
		char c;
		cout << "						search result in ./search_record" << endl;
		cout << "						1.search by LinkID" << endl;
		cout << "						2.search by classnum" << endl;
		cout << "						3.search by forknum" << endl;
		cout << "						4.search by roadname" << endl;
		cout << "						5.search by sql language" << endl;
		cout << "						6.search by roadname(less memory)" << endl;
		cout << "						0.quit" << endl;
		cin >> c;
		while ((b = getchar()) != '\n' && b != EOF);
		int fg = 0;
		if (c <= '9' && c >= '0')
		{
			c = c - '0';
			start = clock();
			switch (c)
			{
			case 1://顺序搜索LinkID
			{
				cout << "please input you search LinkId" << endl;
				//scanf_s("%u", &value);
				cin >> value;
				fg = 0;
				while ((b = getchar()) != '\n' && b != EOF)//清输入缓冲区
				{
					if (b <= '0' || b >= '9')
						fg = 1;
				}
				if (fg == 1)
				{
					cin.clear();////修复输入流,cin.fail=0
					printf("please input correct num\n");
					break;
				}

				sprintf_s(query,100,"%s LinkId=%s;", query, value);
				res=m_mysql->query(query);//获取结果，并显示或存入
				break;
			}
			case 2:
			{
				cout << "please input you search classnum" << endl;
				//scanf_s("%u", &value);
				cin >> value;
				fg = 0;
				while ((b = getchar()) != '\n' && b != EOF)//清输入缓冲区
				{
					if (b <= '0' || b >= '9')
						fg = 1;
				}
				if (fg == 1)
				{
					cin.clear();////修复输入流,cin.fail=0
					printf("please input correct num\n");
					break;
				}
				sprintf_s(query, 100, "%s classnum=%s;", query, value);
				res=m_mysql->query(query);//获取结果，并显示或存入
				break;
			}
			case 3:
			{
				cout << "please input you search forknum" << endl;
				//scanf_s("%u", &value);
				cin >> value;
				fg = 0;
				while ((b = getchar()) != '\n' && b != EOF)//清输入缓冲区
				{}//cin.clear();
				sprintf_s(query, 100, "%s frokroad=%s;", query, value);
				res=m_mysql->query(query);//获取结果，并显示或存入
				break;
			}
			case 4:
			{
				cout << "please input you search roadname" << endl;
				//scanf_s("%u", &value);
				cin >> value;
				fg = 0;
				while ((b = getchar()) != '\n' && b != EOF){}//清输入缓冲区
				sprintf_s(query, 100, "%s name='%s';", query, value);
				res=m_mysql->query(query);//获取结果，并显示或存入
				break;
			}
			case 5://遇到空格就停止怎么？？getline()
			{
				cout << "please input you search sql language" << endl;
				//scanf_s("%u", &value);
				cin.getline(value,200); //>> value;//getline()
				fg = 0;
				//while ((b = getchar()) != '\n' && b != EOF){}//清输入缓冲区
				res=m_mysql->query(value);//获取结果，并显示或存入
				break;
			}
			case 6:
			{
				cout << "please input you search roadname" << endl;
				//scanf_s("%u", &value);
				cin >> value;
				fg = 0;
				while ((b = getchar()) != '\n' && b != EOF) {}//清输入缓冲区
				sprintf_s(query, 100, "%s name='%s';", query, value);
				result = m_mysql->query(query, 1);//获取结果，并显示或存入	
				break;
			}
			case 0:
				flag = 1;
				break;
			default:
				flag = 1;
				break;
			}
		if (flag == 1)
		{
			delete[] value;
			delete[] query;
			break;
		}
		if (c >= 1 && c <= 5)
		{
			end = clock();
			writefile(res, "./search_record");
			tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
			cout << "search cost " << tm << " ms" << endl;
		}
		else
		{
			end = clock();
			writefile(result, "./search_record",1);
			tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
			cout << "search cost " << tm << " ms" << endl;
		}
		}
	}
}

//调用writefile
void MysqlManager::mysqlupdate(const char* filename)
{	
	//从mysql中读取数据并输出到char文件
		ofstream fp;
		fp.open(filename, ios::out | ios::trunc);
		//FILE* fp;
		//errno_t err = fopen_s(&fp, pathname, "w+");
		QueryResult *res = m_mysql->query("select * from roadinfo");
		writefile(res,"./new_record");
		fp.close();
		cout << filename << " file update success" << endl;
}

//mysql api读取2种方式，一种一次全读，一种一条一条读，应一条一条读，并按行输出

bool MysqlManager::mysqlbupdate(const char* filename)
{
	ofstream fp;
	//errno_t err = fopen_s(&fp, pathname, "wb");
	fp.open(filename, ios::binary | ios::trunc);
	int ret = 0;
	unsigned int trint = 0;
	unsigned short trshort = 0;
	char buf2[2];
	char buf3[4];
	unsigned short a = 0x00ff;
	unsigned int b = 0x000000ff;
	node p;
	QueryResult* pResult = m_mysql->query("select * from roadinfo");
	if (pResult == NULL)
		return false;
	//FILE* fp;
	//errno_t err = fopen_s(&fp, pathname, "w+");
	while (true)
	{
		Field* pRow = pResult->fetch();
		if (pRow == NULL)
			return false;
		p.LinkID = pRow[0].getInt32();
		p.classnum = pRow[1].getInt16();
		p.forkroad = pRow[2].getInt16();
		p.hasflag = pRow[3].getBool();
		p.information = pRow[4].getInt32();
		string temp = pRow[5].getString();
		p.record_size = pRow[6].getInt16();
		p.roadsize = pRow[7].getInt16();
		
		trshort = p.record_size;
		for (int i = 0; i < 2; i++)
		{
			buf2[i] = (trshort >> ((1 - i) * 8)) & a;
		}
		//必须反序 int->buf

		//if (!(ret = fwrite(buf2, 2, 1, fp)))
		fp.write(buf2, 2);
		if (fp.fail())
			perror("get record_size error");

		trint = p.LinkID;
		for (int i = 0; i < 4; i++)
		{
			buf3[i] = (trint >> ((3 - i) * 8)) & b;
		}
		//if (!(ret = fwrite(buf3, 4, 1, fp)))
		fp.write(buf3, 4);
		if (fp.fail())
			perror("get LinkID error");

		trshort = p.roadsize;
		for (int i = 0; i < 2; i++)
		{
			buf2[i] = (trshort >> ((1 - i) * 8)) & a;
		}
		//if (!(ret = fwrite(buf2, 2, 1, fp)))
		fp.write(buf2, 2);
		if (fp.fail())
			perror("get roadsize error");

		trint = p.information;
		for (int i = 0; i < 4; i++)
		{
			buf3[i] = (trint >> ((3 - i) * 8)) & b;
		}
		//if (!(ret = fwrite(buf3, 4, 1, fp)))
		fp.write(buf3, 4);
		if (fp.fail())
			perror("get information error");

		int m = p.record_size - 12;
		if (m != 0)
		{
			fp.write(temp.c_str(), m);
			if (fp.fail())
				perror("get name error");
			//fwrite(record[i]->name, m, 1, fp);
		}
		if (!pResult->nextRow())
			break;
	}
	pResult->endQuery();
	delete pResult;

	fp.close();
	return true;
}

void MysqlManager::mysqlinsert()
{
	char* str = new char[200];
	char* value = new char[150];
	char* name = new char[40];
	int flag = 0;
	strcpy_s(str, 100, "insert into roadinfo values(");
	clock_t start=0, end=0;
	double tm;
	char b;
	char c;
	while (1)
	{
		strcpy_s(str, 100, "insert into roadinfo values(");
		cout << "						1.insert by information " << endl;
		cout << "						2.insert by sql language" << endl;
		cout << "						0.quit" << endl;
		cin >> c;
		while ((b = getchar()) != '\n' && b != EOF);
		int fg = 0;
		if (c <= '9' && c >= '0')
		{
			c = c - '0';
			start = clock();
			switch (c)
			{
			case 1:
			{
				cout << "please input your information:" << endl;
				cout << "LinkID classnum forkroad hasflag name" << endl;
				cout << "Usage: 0 0 0 0 hello" << endl;
				int LinkID=0, forkroad=0, hasflag=0,classnum=0;
				cin >> LinkID >>classnum>> forkroad >> hasflag >> name;
				hasflag = hasflag ? 1 : 0;
				//information record_size roadsize
				int roadsize = strlen(name);
				int record_size = 12 + roadsize+2;//2多出来的 00 00
				int information = ((hasflag & 0x01) << 7) + ((forkroad & 0x07) << 4) + (classnum & 0x0f);
				sprintf_s(str,200, "%s%d,%d,%d,%d,%d,'%s',%d,%d);",str,LinkID,classnum,forkroad,hasflag, information,name, record_size,roadsize);
				int i = 0;
				fg = 0;
				m_mysql->execute(str);//获取结果，并显示或存入
				break;
			}
			case 2:
			{
				cout << "please input your sql language" << endl;
				cin.getline(value, 200);
				fg = 0;
				//调整sql语句
				m_mysql->execute(value);//获取结果，并显示或存入
				break;
			}
			case 0:
			{
				flag = 1;
				break;
			}

			}
		}
		if (flag == 1)
			break;
		end = clock();
		tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
		cout << "insert cost " << tm << " ms" << endl;
	}
	delete[] str;
	delete[] value;
}
void MysqlManager::mysqldelete()
{
	char* str = new char[200];
	char* value = new char[150];
	int flag = 0;
	strcpy_s(str, 100, "delete from roadinfo where LinkID=");
	clock_t start=0, end=0;
	double tm;
	char b;
	char c;
	while (1)
	{
		strcpy_s(str, 100, "delete from roadinfo where LinkID=");
		cout << "						1.delete by LinkID " << endl;
		cout << "						2.delete by sql language" << endl;
		cout << "						0.quit" << endl;
		cin >> c;
		while ((b = getchar()) != '\n' && b != EOF);
		int fg = 0;
		if (c <= '9' && c >= '0')
		{
			c = c - '0';
			start = clock();
			switch (c)
			{
			case 1:
			{
				cout << "please input LinkID to delete:" << endl;
				cin.getline(value, 200);
				fg = 0;
				sprintf_s(str,200, "%s%s;", str, value);
				m_mysql->execute(str);//获取结果，并显示或存入
				break;
			}
			case 2:
			{
				cout << "please input your sql language" << endl;
				cin.getline(value, 200);
				fg = 0;
				m_mysql->execute(value);//获取结果，并显示或存入
				break;
			}
			case 0:
			{
				flag = 1;
				break;
			}
			default:
			{
				flag = 1;
				break;
			}
			}
		if (flag == 1)
			break;
		end = clock();
		tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
		cout << "delete cost " << tm << " ms" << endl;
		}	
	}
	delete[] str;
	delete[] value;
}

void MysqlManager::mysqlmodify()
{
	char* str = new char[200];
	char* value = new char[150];
	char* value2 = new char[150];
	int flag = 0;
	strcpy_s(str, 100, "update roadinfo set ");
	clock_t start = 0, end = 0;
	double tm;
	char b;
	char c;
	while (1)
	{
		strcpy_s(str, 100, "update roadinfo set ");
		cout << "						1.update by LinkID " << endl;
		cout << "						2.update by sql language" << endl;
		cout << "						0.quit" << endl;
		cin >> c;
		while ((b = getchar()) != '\n' && b != EOF);
		int fg = 0;
		if (c <= '9' && c >= '0')
		{
			c = c - '0';
			start = clock();
			switch (c)
			{
			case 1:
			{
				cout << "please input LinkID to update:" << endl;
				cin.getline(value, 200);
				cout << "please input set what to update" << endl;
				cout << "Usage: frokroad=3,name='hello'" << endl;
				cin.getline(value2, 200);
				fg = 0;
				sprintf_s(str, 200, "%s %s where LinkID=%s;", str,value2, value);
				m_mysql->execute(str);//获取结果，并显示或存入
				break;
			}
			case 2:
			{
				cout << "please input your sql language" << endl;
				cin.getline(value, 200);
				fg = 0;
				m_mysql->execute(value);//获取结果，并显示或存入
				break;
			}
			case 0:
			{
				flag = 1;
				break;
			}
			default:
			{
				flag = 1;
				break;
			}
			}
			if (flag == 1)
				break;
			end = clock();
			tm = 1000.0 * ((double)end - (double)start) / CLOCKS_PER_SEC;
			cout << "modify cost " << tm << " ms" << endl;
		}
	}
	delete[] str;
	delete[] value;
	delete[] value2;
}

bool MysqlManager::mysqlshowall()
{
	node p;
	QueryResult *pResult = m_mysql->query("select * from roadinfo;");
	if (pResult == NULL)
		return false;
	//FILE* fp;
	//errno_t err = fopen_s(&fp, pathname, "w+");
	cout << "   Linkid   record_size  roadsize   hasflag  forkroad   classnum       roadname" << endl;
	while (true)
	{
		Field* pRow = pResult->fetch();
		if (pRow == NULL)
			return false;
		
		p.LinkID = pRow[0].getInt32();
		p.classnum = pRow[1].getInt16();
		p.forkroad = pRow[2].getInt16();
		p.hasflag = pRow[3].getBool();
		p.information = pRow[4].getInt32();
		string temp = pRow[5].getString();
		p.record_size = pRow[6].getInt16();
		p.roadsize = pRow[7].getInt16();
		cout << setw(10) << p.LinkID << setw(10) << p.record_size << setw(10) << p.roadsize << setw(10)\
			<< p.hasflag << setw(10) << p.forkroad << setw(10) << p.classnum<< setw(20) << temp << endl;
		if (!pResult->nextRow())
			break;
	}
	pResult->endQuery();
	delete pResult;
	return true;
}

bool MysqlManager::mysqlread(const char* filename)
{
	char* str = new char[1024 * 1024];
	//若要查询需要重新建立连接
	if (!m_mysql->execute("delete from roadinfo;"))
		return false;
	sprintf_s(str, 150, "insert into roadinfo(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize) values");
	//(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize)
	clock_t start = clock();
	ifstream fp;
	//FILE* fp;
	//int err = fopen_s(&fp, filename, "rb");//打开文件
	fp.open(filename, ios::binary);
	if (!fp)
	{
		perror("open file error");
		return false;
	}
	char buf1[2];//保存2字节信息
	char buf2[4];//保存4字节信息
	char buf3[100];//保存道路名字

	node* p = new node();//创建node
	char* name = new char[100];
	p->name = name;
	long long ret = 0;
	//EOF是文本文件结束的标志。在文本文件中，数据是以字符的ASCⅡ代码值的形式存放，
	//ASCII代码值的范围是0～255，EOF的16进制代码为0xFF(十进制为-1)，因此可以用EOF作为文件结束标志。
	//但是如果文件格式为二进制文件，就不适合用EOF来判断文件到达结尾了，
	//因为有可能文件中也存在一个数据为0xFF，这样当读取到这个数据是，就无法判断是到达文件结尾
	//还是读取到的是文件的内容了。
	int i = 0;
	while (!fp.eof())
	{
		fp.read(buf1, 2);
		ret = fp.gcount();
		//ret = fread(buf1, 2, 1, fp);//读“长度”
		unsigned short size = 0;
		//printf("%d\n", ret);
		if (ret == 2)
		{
			size = trans2(buf1);
			p->record_size = size;
			//size = (unsigned short)buf1;
			//size=25
			//cout << size << endl;
			size -= 2;
			fp.read(buf2, 4);
			//if (ret = fread(buf2, 4, 1, fp))//读道路编号
			if ((ret = fp.gcount()) == 4)
			{
				if (ret != 4)//错误处理
				{
					delete(p);
					perror("get LinkId error");
					return false;
				}
				size -= 4;
				unsigned int LinkId = trans4(buf2);
				//cout << "result:"<<LinkId<<endl;
				//unsigned int LinkId = (unsigned int)buf2[3] + (unsigned int)(buf2[2] << 8) + \
				//	(unsigned int)(buf2[1] << 16) + (unsigned int)(buf2[0] << 24);
				p->LinkID = LinkId;
				//cout << LinkId << endl;
			}
			//if (ret = fread(buf1, 2, 1, fp))//读道路名称长度
			fp.read(buf1, 2);
			if ((ret = fp.gcount()) == 2)
			{
				unsigned short namesize = 0;
				namesize = trans2(buf1);
				//namesize=(unsigned int)buf1[1] + (unsigned int)(buf1[0] << 8);
				p->roadsize = namesize;
				size -= 2;
			}
			else
			{
				delete(p);
				perror("get namesize error");
				return false;
			}

			//if (ret = fread(buf2, 4, 1, fp))//读道路相关信息
			fp.read(buf2, 4);
			if ((ret = fp.gcount()) == 4)
			{
				unsigned int infor = trans4(buf2);
				p->information = infor;
				bool hasflag = buf2[3] & 0x80;
				if (hasflag != 0)
					p->hasflag = true;
				else
					p->hasflag = false;

				short forknum = (buf2[3] & 0x70) >> 4; p->forkroad = forknum;
				short classnum = buf2[3] & 0x0f; p->classnum = classnum;
				size -= 4;
			}
			else
			{
				delete(p);
				perror("get road information error");
				return false;
			}
			//if (ret = fread(buf3, size, 1, fp))//获取道路名称
			fp.read(buf3, size);
			if (size != 0)
			{
				if ((ret = fp.gcount()) != size)
				{
					delete(p);
					cout << "rest size:" << size << endl;
					perror("get name error");
					//注意释放内存再退出
					return false;
				}
				size = 0;
				size_t len = strlen(buf3);
				strcpy_s(name, strlen(buf3) + 1, buf3);
				//printf("len:%d\n", len);
				name[len] = '\0';
			}
			else
			{
				strcpy_s(name, 10, "null");
			}
			//格式化一条数据，并加入sql语句
			//sprintf_s(str, 200,"insert into roadinfo values( %d,%d,%d,%d,%d,%d,%d,%s);", record[i]->LinkID,record[i]->record_size
			//	,record[i]->roadsize,record[i]->hasflag,record[i]->forkroad,record[i]->classnum,record[i]->information,record[i]->name);

		// record_size Linkid    roadsize   hasflag  forkroad   classnum       roadname
			if (i > 0 && i % 1000 == 0)
			{
				sprintf_s(str, 1024 * 1024, "%s(%d,%d,%d,%d,%d,'%s',%d,%d);", str, p->LinkID, p->classnum
					, p->forkroad, p->hasflag, p->information, p->name, p->record_size, p->roadsize);
				if (m_mysql->execute(str) == false)//插入数据,已经有了
				{
					cout << "data already exists" << endl;
					return false;
				}
				sprintf_s(str, 150, "insert into roadinfo(LinkID,classnum,forkroad,hasflag,information,name,record_size,roadsize) values");
			}
			else
				sprintf_s(str, 1024 * 1024, "%s(%d,%d,%d,%d,%d,'%s',%d,%d),", str, p->LinkID, p->classnum
					, p->forkroad, p->hasflag, p->information, p->name, p->record_size, p->roadsize);
			i++;
		}
		else
		{
		cout << "The " << i + 1 << "record get error" << endl;
			//perror(" record get failed");
		}
	}
	if (strlen(str) > 101)
	{
		int k = strlen(str);
		str[k - 1] = ';';
		if (m_mysql->execute(str) == false)//插入数据,已经有了
		{
			cout << "insert error" << endl;
			return false;
		}
	}
	cout << "get data success" << endl;

	clock_t end = clock();
	cout << "cost " << ((double)end - (double)start) / CLOCKS_PER_SEC << " sec" << endl;
	fp.close();
	delete[] name;
	delete p;
	//mysql.~CMysql();
	return true;
}


bool MysqlManager::writefile(QueryResult* pResult,const char* pathname)
{
	ofstream fp;
	fp.open(pathname, ios::out | ios::trunc);
	if (pResult == NULL)
		return false;
	//FILE* fp;
	//errno_t err = fopen_s(&fp, pathname, "w+");
	fp << "    Linkid  record_size  roadsize    hasflag   forkroad   classnum         roadname" << endl;
	while (true)
	{
		Field* pRow = pResult->fetch();
		if (pRow == NULL)
			return false;
		node p;
		p.LinkID = pRow[0].getInt32();
		p.classnum = pRow[1].getInt16();
		p.forkroad = pRow[2].getInt16();
		p.hasflag = pRow[3].getBool();
		p.information = pRow[4].getInt32();
		string temp=pRow[5].getString();
		p.record_size = pRow[6].getInt16();
		p.roadsize = pRow[7].getInt16();

		//将p写入文件
		fp << setw(10) << p.LinkID << setw(10) << p.record_size << setw(10) << p.roadsize\
			<< setw(10) << p.hasflag << setw(10) << p.forkroad << setw(10) << p.classnum
			<< setw(25) << temp << endl;
		if(!pResult->nextRow())
			break;
	}
	pResult->endQuery();
	delete pResult;
	fp.close();
	cout << pathname << " file update success" << endl;
	return true;
}

bool MysqlManager::writefile(MYSQL_RES* result, const char* pathname,int k)
{
	(void)k;
	ofstream fp;
	fp.open(pathname, ios::out | ios::trunc);
	if (result == NULL)
		return false;
	//FILE* fp;
	//errno_t err = fopen_s(&fp, pathname, "w+");
	fp << "    Linkid  record_size  roadsize    hasflag   forkroad   classnum         roadname" << endl;
	while (true)
	{
		MYSQL_ROW pRow = mysql_fetch_row(result);
		if (pRow == NULL)
			return false;
		node p;
		p.LinkID = atoi(pRow[0]);
		p.classnum = atoi(pRow[1]);
		p.forkroad = atoi(pRow[2]);
		p.hasflag = atoi(pRow[3]);
		p.information = atoi(pRow[4]);
		char* temp = pRow[5];
		p.record_size = atoi(pRow[6]);
		p.roadsize = atoi(pRow[7]);

		//将p写入文件
		fp << setw(10) << p.LinkID << setw(10) << p.record_size << setw(10) << p.roadsize\
			<< setw(10) << p.hasflag << setw(10) << p.forkroad << setw(10) << p.classnum
			<< setw(25) << temp << endl;
	}
	mysql_free_result(result);
	fp.close();
	cout << pathname << " file update success" << endl;
	return true;
}

void MysqlManager::readRecord(Record& record)
{
	record.clear();
	QueryResult* pResult = m_mysql->query("select * from roadinfo;");
	if (pResult == NULL)
		return;
	//FILE* fp;
	//errno_t err = fopen_s(&fp, pathname, "w+");
	int i = 0;
	while (true)
	{
		node* p = new node();
		Field* pRow = pResult->fetch();
		if (pRow == NULL)
			return;
		p->LinkID = pRow[0].getInt32();
		p->classnum = pRow[1].getInt16();
		p->forkroad = pRow[2].getInt16();
		p->hasflag = pRow[3].getBool();
		p->information = pRow[4].getInt32();

		//如何在退出时销毁内存占用
		size_t len = strlen(pRow[5].getString().c_str());
		char* name = new char[len + 1];
		strcpy_s(name, len + 1, pRow[5].getString().c_str());
		//printf("len:%d\n", len);
		name[len] = '\0';

		p->name = name;
		p->record_size = pRow[6].getInt16();
		p->roadsize = pRow[7].getInt16();
		record.push_back(p);
		i++;
		if (!pResult->nextRow())
			break;
	}
	pResult->endQuery();
	delete pResult;
	cout << "Record update success" << endl;
}