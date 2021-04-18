#include"memorytest.h"
/**
 * 程序功能：存储系统一致性检验
 * 本文件：存放main函数，存放业务代码
 * 时间：2021年3月9日20点40分
**/ 
int* queue=NULL;//用于写线程获取未被写的块,1表示可写，0表示不可写，第i块对应4096+(blocksize*i)偏移处
int testing = 0;//是否正在检验
int rest_num = 0;//剩余块数
int ttyfd = 0;//用于后台启动时，信号USER1的函数向终端写输出
int fd = 0;//写入文件描述符
int randx = 0;//随机数
int configfd = 0;//配置文件fd
int blocksize = 8 * SECTOR_SIZE;//块大小,默认8扇区;
long size = 1024 * 1024 * 1024;//文件大小,默认1GB
int blocknum = 0;//块数
int verify = 1;//是否边写边校验，0代表只写，写完后再进行校验，1表示写的过程中随机校验
int int_size = sizeof(int);//int的大小
int falsecolor = 0;//错误的显示颜色
int openD = 1;//是否通过O_DIRECT方式打开  【默认O_DIRECT打开 1 】
int daemonon = 0;//是否后台运行，1表示后台运行
int threadnum=1;//线程数
int errorsum = 0;//错误总数
int lck = 0;//文件锁
Mutex wlock;//保护lseek和write原子
Mutex mutex;//配合finish_cond和verify_finish条件变量
Mutex mutex1;//保护que队列
Mutex mutex3;//配合verify_cond条件变量
Mutex mutex4;//保证刷新随机数和重置lseek原子
Mutex mutex6;//配合stop_cond条件变量

Cond work_cond;//唤醒所有写工作线程
Cond stop_cond;//其他写线程唤醒主线程，须全部写线程完成才唤醒主线程
Cond verify_cond;//检验线程等待
Cond finish_cond;//主线程等待检验线程
Cond verify_finish;//用于随机检验，等待检验完成，主线程、写线程
#ifndef WIN32 
pthread_t pid;
#endif
//主函数，功能验证存储系统的一致性
int main(int argc,char **argv)
{
	if (file_lock(&lck) == 0)//文件锁
		return 0;
	thread_init();//初始化线程同步工具
	int ret = 0;//线程数,返回值
	char version[10];//版本
	strcpy(version,"1.0.0");
	char filename[20];//文件名
	strcpy(filename,"a.file");
	srand((unsigned int)time(0));
	//注册信号，便于退出时写入config
	//令非正常退出也同样导向exit正常退出，从而调用at_exit指定函数，写配置文件，但kill -9不可用
	atexit(server_on_exit);
	signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
	signal(SIGABRT, signal_crash_handler);// SIGABRT，由调用abort函数产生，进程非正常退出
	if ((ret = my_getopt(argc, argv, version, &threadnum)) == 0)//提取程序参数
		return 0;
	Daemon_init();//后台运行
	blocknum = (size-4*1024)/ blocksize;//剩余多少块数
	queue=(int*)malloc(sizeof(int)*blocknum);//初始化队列
	memset(queue,0,sizeof(int)*blocknum);
	char* memptr=(char*)Malloc(FIRSTBLOCK_NUM,openD);//4K块内存缓存
	fd = Open(filename,openD,&fd);//打开文件
	if(fd<=0)
	{
		printf("open error ,errno=%d\n",errno);
		return 0;
	}
	first_verify();//根据配置文件验证上次的
	close(fd);//关掉文件重新打开的目的是，第二次打开以TRUNC方式打开，避免文件一直很大
	fd = Open_trunc(filename,openD,&fd);
	if(fd<=0)
	{
		printf("open error ,errno=%d\n",errno);
		return 0;
	}
	Pthread_create(hd_write_verify_thread);//开启检验线程
	for (int i = 0; i < threadnum; ++i)//开启多个写线程
	{
		Pthread_create(hd_write_thread);
	}
	mainfunc(memptr);//主循环，写4k块和获取随机数
	close(fd);
	free(queue);
	free(memptr);
	return 0;
}

#ifndef WIN32
void *hd_write_thread(void* args)//写线程，多个线程同时写一个文件，按随机数写，无参数，无返回值
#else
void hd_write_thread(void* args)//写线程，多个线程同时写一个文件，按随机数写
#endif
{
	int n=-1;
	char* tmpptr = (char*)Malloc(blocksize,openD);//剩余块内存缓存，以1块为单位
	memset(tmpptr,1,blocksize);
	while(1)
	{
		//加锁从队列中获取一个块
		n=-1;
		Mutexlock(&mutex1);
		while (n == -1)
		{
			for (int i = 0; i < blocknum; i++)
			{
				if (queue[i] == 1)
				{
					queue[i] = 0; n = i;
					break;
				}
			}
			if (n == -1)
				Cond_Wait(&work_cond, &mutex1);
		}
		Mutexunlock(&mutex1);
		//开始取得该块对应的随机数，并写入
		int xtemp=randx+n+1;
		buf_write_num(xtemp, tmpptr, blocksize,SECTOR_SIZE);//填充缓冲区
		Mutexlock(&wlock);//原子写规定位置
		long seek=lseek(fd,4*1024+blocksize*n,SEEK_SET);
		int ret = 0;
		int least = blocksize;
		while (ret != blocksize)
		{
			if(least!=0)
			{
				ret+=write(fd, tmpptr+ret, least);
				least = blocksize-ret;
			}
		}
		if(verify==1)//如果是随机校验
		{
			if (rand() % 10 == 0)
			{
				Mutexlock(&mutex3);
				testing=1;
				Cond_Signal(&verify_cond);//唤醒检验线程
				Mutexunlock(&mutex3);
				Mutexlock(&mutex);
				while(testing)
				{
					Cond_Wait(&verify_finish,&mutex);//等待检验线程唤醒
				}
				Mutexunlock(&mutex);
			}
		}
		Mutexlock(&mutex6);
		rest_num--;
		Cond_Signal(&stop_cond);//唤醒主线程
		Mutexunlock(&mutex6);
		Mutexunlock(&wlock);
	}
	free(tmpptr);
}
#ifndef WIN32
void *hd_write_verify_thread(void *args)//检验线程，无参数，无返回值
#else
void hd_write_verify_thread(void* args)
#endif
{
	if(verify==0)
	{
		//写完后在校验
		verify_after_finish();
	}
	else
	{
		//写的过程中随即校验，等验证完其他线程再写
		verify_after_write();
	}
}

//写完后校验
void verify_after_finish()
{
	char *buffer=Malloc(blocksize,openD);//用于读取文件blocksize大小
	char *buffer1=Malloc(4*1024,openD);//用于读取文件4K块大小
	int n=0;
	int color=falsecolor;
	errorsum=0;//本次错误总数
	int ret=0;
	while(1)
	{
		Mutexlock(&mutex3);
		while(testing==0)
			Cond_Wait(&verify_cond,&mutex3);//等待被唤醒
		Mutexunlock(&mutex3);
		testing=1;
		int errornum=0;//本次错误数
		lseek(fd,0,SEEK_SET);
		ret = 0;
		//校验前4K块
		int least = FIRSTBLOCK_NUM;
		while(ret!=FIRSTBLOCK_NUM)
		{	
			if (ret != -1)
			{
				ret += read(fd, buffer1+ret, least);
				least= FIRSTBLOCK_NUM-ret;
				//printf("first read ret :%d\n",ret);
			}
		}
		for(int i=0;i<FIRSTBLOCK_NUM;i=i+int_size)
		{
			if((trans4((buffer1+i)))==randx)
			{
				continue;
			}
			else
			{
				errornum++;errorsum++;
				printferr(i,randx,(trans4((buffer1+i))),color,daemonon);
			}
		}
		//校验其他块	
		for(int i=1;i<=blocknum;i++)
		{
			ret = 0;
			int least = blocksize;
			while (ret != blocksize)
			{
				if (ret != -1)
				{
					ret += read(fd, buffer+ret, least);
					least = blocksize-ret;
				}
			}
			for(int j=0;j<blocksize;j=j+int_size)
			{
				if((trans4((buffer+j)))==randx+i)
					continue;
				else
				{
					errornum++;errorsum++;
					printferr(4*1024+(i-1)*blocksize+j,randx+i,(trans4((buffer+j))),color,daemonon);
				}
			}
		}
		printf("the %d time verify finish，error time：%d\n",++n,errornum);
		Mutexlock(&mutex);
		testing=0;
		Cond_Signal(&finish_cond);//唤醒主线程
		Mutexunlock(&mutex);
	}
	free(buffer);
	free(buffer1);
}
//写的过程中随机校验
void verify_after_write()
{
	char *buffer=Malloc(blocksize,openD);//用于读取文件blocksize大小
	char *buffer1=Malloc(4*1024,openD);//用于读取文件4K块大小
	int color=falsecolor;
	errorsum=0;//本次错误总数
	while(1)
	{
		Mutexlock(&mutex3);
		while(testing==0)
			Cond_Wait(&verify_cond,&mutex3);//某个write的时候，被唤醒
		Mutexunlock(&mutex3);
		testing=1;
		int temp=0;
		long point=lseek(fd,0,SEEK_CUR);//获取当前写指针位置
		if(point==0)//如果当前offset=0
			continue;
		else if(point==4*1024)//如果当前offset=4096，说明要校验前4K块
		{
			temp=randx;
			Read(fd,buffer1,FIRSTBLOCK_NUM);
			for(int i=0;i<FIRSTBLOCK_NUM;i=i+int_size)
			{
				if((trans4((buffer1+i)))==randx)
					continue;
				else
				{
					errorsum++;
					printferr(i,randx,(trans4((buffer1+i))),color,daemonon);
				}
			}
		}
		else if(point>4*1024)//校验非4K块
		{
			Read(fd,buffer,blocksize);
			temp=randx+(point-4*1024)/(blocksize);
			for(int j=0;j<blocksize;j=j+int_size)
			{
				if((trans4((buffer+j)))==temp)
					continue;
				else
				{
					errorsum++;
					printferr(point-blocksize+j,temp,(trans4((buffer+j))),color,daemonon);
				}
			}
		}
			printf("correct in offset %ld,num=%d\n",point-blocksize,temp);
		Mutexlock(&mutex);
		testing=0;
		Cond_Signal(&verify_finish);
		Mutexunlock(&mutex);
	}
	free(buffer);
	free(buffer1);
}
//主函数的循环，主线程取随机数 和 写4k块
void mainfunc(char* memptr)
{
	while (1)//主线程取随机数 和 写4k块
	{
		//一开始其他线程和检验线程都处在cond_wait
		Mutexlock(&mutex4);
		lseek(fd, 0, SEEK_SET);
		randx = rand() % 1000;//获取随机数，启动时获取随机数,写入配置文件？
		Mutexunlock(&mutex4);
		rest_num = blocknum;//初始化队列剩余块
		first_write(memptr);//写4K块
		//让队列可用
		Mutexlock(&mutex1);
		for (int i = 0; i < blocknum; ++i)
			queue[i] = 1;
		Cond_SignalAll(&work_cond);//通知其他线程
		Mutexunlock(&mutex1);
		Mutexlock(&mutex6);
		while (rest_num)
			Cond_Wait(&stop_cond, &mutex6);//等待其他写线程写完
		Mutexunlock(&mutex6);
		if (verify == 0)
		{
			Mutexlock(&mutex3);
			testing = 1;
			Cond_Signal(&verify_cond);//通知检验线程检验
			Mutexunlock(&mutex3);
			Mutexlock(&mutex);
			while (testing != 0)//检验线程通知主线程
				Cond_Wait(&finish_cond, &mutex);
			Mutexunlock(&mutex);
		}
	}
}
//获取程序的参数 -S、-s、-T、-V、-d、-D、-c、-h、-v
int my_getopt(int argc, char** argv, char* version, int *threadnum)
{
	if (argc >= 1)
	{
		//获取块大小
		int ch = 0;
		while ((ch = getopt(argc, argv, "hvS:s:c:dDVT:")) != -1)
		{
			switch (ch)
			{
			case 'h'://help
			{
				Printhelp(); return 0;
				break;
			}
			case 'v'://version
				printf("当前版本 %s\n", version); return 0;
				break;
			case 'S'://size 指定校验文件的大小 【默认1GB】
			{
				size = getsize(optarg);
				if (size <= (long)0 || size > 1*1024*1024*1024)
				{
					printf("usage:  0 <size <= 1G\n"); return 0;
				}
				break;
			}
			case 's'://sector  # 每次写的块大小(扇区个数(扇区大小为512byte)),[1,128] 【默认是8，即4K】
			{
				blocksize = atoi(optarg) * SECTOR_SIZE;
				if (blocksize <= 0 || blocksize > 128 * SECTOR_SIZE)
				{
					printf("usage:1*SECTOR <=blocksize <=128*SECTOR,SECTOR_SIZE=512\n"); return 0;
				}
				break;
			}
			case 'c'://color   # 显示不通过的颜色，比如将错误信息显示红色，将错误的地址高亮，将头部信息通过绿色显示等 【默认显示颜色】
			{
				char* color = optarg;
				falsecolor = setcolor(color);
				break;
			}
			case 'd'://daemonon  # 后台运行，能通过日志或信号查看当前运行情况(如：正在写哪些偏移，校验情况如何)         【默认前台运行】
			{
				daemonon = 1;
				break;
			}
			case 'D'://direct  # 是否通过O_DIRECT方式打开 【默认O_DIRECT打开】
			{
				//仅Linux下支持,open(filename,O_DIRECT);
				openD = 0;
				break;
			}
			case 'V'://verify  # 是否边写边校验，0代表只写，写完后再进行校验，1表示写的过程中随机校验                 【默认边写边校验】
			{
				verify = 0;
				break;
			}
			case 'T'://-thread  # 线程数，多线程写入, [1-10] 【默认单线程】
			{
				*threadnum = atoi(optarg);
				break;
			}
			}
		}
	}
	return 1;
}
//是否后台运行
void Daemon_init()
{
#ifndef WIN32
	if (daemonon)
		daemon_init(&ttyfd,&lck);
#endif
}
//写4K块
void first_write(char* memptr)
{
	if (memptr == NULL)
		return;
	int ret = 0;
	buf_write_num(randx,memptr,FIRSTBLOCK_NUM,SECTOR_SIZE);//写前4k内容，乘法增长2 4 8 16 32
	int least = FIRSTBLOCK_NUM;
	if (verify == 0)//写完全部后检验
		while (ret != FIRSTBLOCK_NUM)
		{
			ret += write(fd, memptr+ret, least);//写第一个4k块
			least = FIRSTBLOCK_NUM-ret;
			//printf("first:ret=%d\n", ret);
		}
	else//随机写且检验
	{
		while (ret != FIRSTBLOCK_NUM)
		{
			ret += write(fd, memptr, least);
			least = FIRSTBLOCK_NUM-ret;
			//			printf("first:ret=%d\n", ret);
		}
		if (rand() % 10 == 0)
		{
			Mutexlock(&mutex3);
			testing = 1;
			Cond_Signal(&verify_cond);
			Mutexunlock(&mutex3);

			Mutexlock(&mutex);
			while (testing)
				Cond_Wait(&verify_finish, &mutex);
			Mutexunlock(&mutex);
		}
	}
}


//获取配置文件、参数：writen 是否写过、randconfig 随机数、offset 上次偏移、blocksize 上次块大小
int getconfig(int *writen,int *randconfig,long *offsetconfig,int *blocksizeconfig,int *threadnum)
{
	int configfd = open("config", O_RDWR|O_CREAT,0666);
	char config[230];
	if(configfd!=-1)
	{
		int ret=read(configfd,config,200);
		if(ret<=0)
		{
			printf("red config file error\n");
		}
		else
		{
			char *p=NULL;
			p=strstr(config,"writen=");
			p=p+7;
			*writen=*p-'0';
			
			p=strstr(config,"rand=");
			p=p+5;
			int y=0,m=0;
			while(p[m]<='9'&&p[m]>='0')
			{
				y=y*10+p[m]-'0';
				m++;
			}
			*randconfig=y;
			
			p=strstr(config,"offset=");
			p=p+7;
			long q=0;m=0;
			while(p[m]<='9'&&p[m]>='0')
			{
				q=q*10+p[m]-'0';
				m++;
			}
			*offsetconfig=q;
			
			p=strstr(config,"blocksize=");
			p=p+10;
			y=0,m=0;
			while(p[m]<='9'&&p[m]>='0')
			{
				y=y*10+p[m]-'0';
				m++;
			}
			*blocksizeconfig=y;

			p=strstr(config,"threadnum=");
			p=p+10;
			y=0,m=0;
			while(p[m]<='9'&&p[m]>='0')
			{
				y=y*10+p[m]-'0';
				m++;
			}
			*threadnum=y;
		}
	}
	else
		printf("config file error\n");
	return configfd;
}
//程序的第一次校验，如果手动修改文件，可以在此函数中检验出问题
void first_verify()
{
	//读取配置文件
	int writen = 0;//是否写过
	int randnum = 0;//上次随机数
	long int offset = 0;//上次偏移
	int blocksize = 0;//上次块大小
	int threadnum=0;
	configfd = getconfig(&writen, &randnum, &offset, &blocksize,&threadnum);//获取配置文件信息
	//printf("config %d %d %ld %d\n",writen,randconfig,offsetconfig,blocksizeconfig);
	if (threadnum > 1)//应该是上次的threadnum，即threadnum写入配置文件
	{
		//因为线程数大于1，则每次写的块位置不是顺序的，有可能先写后边的块，而此
		//时退出，则前面的块不保证完全写完，在某种情况下会出错，解决方案是记录每个线程的offset，
		//但无法验证已经检验过的所有块,所以多线程情况下，只有过程中的检验具有验证价值。
		printf("if threadnum>1,can't verify last time\n");
		return;
	}
	int x=randnum;
	int ret=0;
	//写完后在校验
	if(writen==0||offset<=0||blocksize<=0)
	{
		printf("verify pass\n");
		return;
	}
	if(writen==1)
	{
		int errornum=0;//本次错误数
		int blocknum=(offset-4*1024)/blocksize;
		char *buffer1=Malloc(sizeof(char)*FIRSTBLOCK_NUM,openD);
		char *buffer=Malloc(sizeof(char)*blocksize,openD);
		lseek(fd,0,SEEK_SET);
		ret = 0;
		ret=read(fd,buffer1,FIRSTBLOCK_NUM);
		if(ret<=0)
		{
			printf("errno= %d ,read error\n",errno);
		}
		for(int i=0;i<FIRSTBLOCK_NUM;i=i+int_size)
		{
			if((trans4((buffer1+i)))==x)
				continue;
			else
			{
				errornum++;
				printferr(i,x,(trans4((buffer1+i))),falsecolor,daemonon);
			}
		}
		if(blocknum>0)
		for(int i=1;i<=blocknum;i++)
		{
			ret=read(fd,buffer,blocksize);
			if(ret<=0)
			{
				printf("errno= %d ,read error\n",errno);
			}
			for(int j=0;j<blocksize;j=j+int_size)
			{
				if((trans4((buffer+j)))==x+i)
					continue;
				else
				{
					errornum++;
					printferr(4*1024+(i-1)*blocksize+j,x+i,(trans4((buffer+j))),falsecolor,daemonon);
				}
			}
		}
		if(errornum==0)
			printf("verify pass in init verify\n");
		else
			printf("init verify finish，error time：%d\n",errornum);
	}
}


//信号处理函数，可在后台运行过程中，向进程发送SIGUSR1，实时显示信息
void sig_handler(int sig)
{
	/*保留原有的errno，在函数最后回复，以保证函数的可重入*/
	char str[50];
	sprintf(str, "\nverify in a.file offset:%ld,errorsum=%d\n", lseek(fd, 0, SEEK_CUR), errorsum);
	write(ttyfd, str, strlen(str));
}

//退出时运行，写配置文件
void server_on_exit(void)
{
#ifdef WIN32
		Mutexlock(&mutex4);
#endif
	//randconfig blocksizeconfig offsetconfig
	long offsetconfig=lseek(fd,0,SEEK_CUR)-blocksize;
	close(configfd);
	int configfd=open("./config",O_TRUNC|O_RDWR);
	int randconfig=randx;
	int blocksizeconfig=blocksize;
	char str[100];
	sprintf(str,"writen=1\n");
	write(configfd,str,strlen(str));
	sprintf(str,"rand=%d\n",randconfig);
	write(configfd,str,strlen(str));
	sprintf(str,"offset=%ld\n",offsetconfig);
	write(configfd,str,strlen(str));
	sprintf(str,"blocksize=%d\n",blocksizeconfig);
	write(configfd,str,strlen(str));
	sprintf(str,"errorsum=%d\n",errorsum);
	write(configfd,str,strlen(str));
	sprintf(str,"threadnum=%d\n",threadnum);
	write(configfd,str,strlen(str));
	close(configfd);
	close(fd);
	close(lck);
	remove("a.lock");
	printf("in this test errorsum=%d\n", errorsum);
	#ifdef WIN32
		LeaveCriticalSection(&mutex4);
	#endif
	// Pthread_mutex_destroy(&mutex);
	// Pthread_mutex_destory(&mutex1);
	// Pthread_mutex_destory(&mutex3);
	// Pthread_mutex_destory(&mutex4);
	// Pthread_mutex_destory(&mutex6);
	// Pthread_cond_destroy(&mutex);
	// Pthread_cond_destroy(&mutex1);
	// Pthread_cond_destroy(&mutex3);
	// Pthread_cond_destroy(&mutex4);
	// Pthread_cond_destroy(&mutex6);
	// pthread_join(&pid,NULL);
}

//初始化线程与并发资源
void thread_init()
{
	Mutex_Init(&wlock);
	Mutex_Init(&mutex);
	Mutex_Init(&mutex1);
	Mutex_Init(&mutex3);
	Mutex_Init(&mutex4);
	Mutex_Init(&mutex6);
	Cond_Init(&work_cond);
	Cond_Init(&stop_cond);
	Cond_Init(&verify_cond);
	Cond_Init(&finish_cond);
	Cond_Init(&verify_finish);
}

//后台进程启动，不可单测
#ifndef WIN32
int daemon_init(int *ttyfd,int *lck)//const char *pname,int facility
{
	pid_t pid;
	//�رո�����
	if ((pid = fork()) < 0)
		return -1;
	else if (pid)
		exit(0);
	//���ûỰ
	if (setsid() < 0)
	{
		return -1;
	}
	signal(SIGHUP, SIG_IGN);
	//�ٴ�fork����ֹ�ӽ��̴��ն˻�ȡ����
	if ((pid = fork()) < 0)
		return -1;
	else if (pid)
		exit(0);
	//�ļ���

	*lck = open("a.lock", O_RDONLY | O_CREAT | O_EXCL | O_TRUNC, 0666);
	if (*lck < 0)
	{
		if (errno == EEXIST)
		{
			printf("./memorytest is runing and can't open again\n");
			return 0;
		}
	}

	addsig(SIGUSR1);
	//�޸Ľ�����Ŀ¼
	chdir("./");
	//�رմ��ļ�
	dup2(1, 64);
	for (int i = 0; i < 64; i++)
		close(i);
	//�ض�λ��׼���롢���������
	open("/dev/null", O_RDONLY);
	int b = open("a.txt", O_CREAT|O_RDWR | O_TRUNC,0666);//����ʹ���д��memmorytest.log
	open("a.txt", O_RDWR);
	*ttyfd = dup(64);
	//printf("x=%d\n",x);
	close(64);
	//����signal��USER1��USER2��ʱ��ȡ��֤���
	//printf("x=%d\n",x);
	//openlog(pname,LOG_PID,facility);
	return b;
}
#endif

//添加信号，不可单测
#ifndef WIN32
void addsig(int sig)
{
	struct sigaction sa;
	memset(&sa, '\0', sizeof(sa));
	sa.sa_handler = sig_handler;
	sa.sa_flags |= SA_RESTART;
	sigfillset(&sa.sa_mask);
	assert(sigaction(sig, &sa, NULL) != -1);//�����ź��뺯��
}
#else
void addsig(int sig)
{
	signal(sig, sig_handler);
}
#endif

void signal_crash_handler(int sig)
{
	exit(-1);
}

void signal_exit_handler(int sig)//�������쳣�˳�Ҳ��Ϊ����exit�˳����Ӷ�����server_on_exit
{
	exit(0);
}

void Printhelp()
{
	printf("------------------------\n");
	printf("./hd_write_verify usage help\n");
	printf("-h | --help    # 帮助信息\n");
	printf("-v | --version # 版本信息\n");
	printf("-S | --size    # 指定校验文件的大小 \n");
	printf("-s | --sector  # 每次写的块大小(扇区个数(扇区大小为512byte)),[1,128] \n");
	printf("-c | --color   # 显示不通过的颜色，将错误信息显示红色，将错误的地址高亮（仅支持LINUX）                【默认显示颜色】\n");
	printf("-d | --daemon  # 后台运行，能通过日志或信号查看当前运行情况(偏移与验情况如何)（仅LINUX支持）          【默认前台运行】\n");
	printf("-D | --direct  # 是否通过O_DIRECT方式打开（仅LINUX支持）                                              【默认O_DIRECT打开】");
	printf("-V | --verify  # 是否边写边校验，0代表只写，写完后再进行校验，1表示写的过程中随机校验                 【默认边写边校验】\n");
	printf("-T | --thread  # 线程数，多线程写入, [1-10]                                          	              【默认单线程】\n");
	printf("------------------------\n");
}
/*
1.颜色 √
2.后台 √、日志 √、信号 √
3.O_DIRECT √
4.整体流程图 √
5.随机数如何应对，config √
6.线程 √
7.windows √

O_DIRECT:
也就是说O_DIRECT只是不使用内核的高速缓存，直接读写磁盘，导致我们自己使用的缓存必须是
磁盘扇区对齐的（512*x），而且读写write、read也得是对齐的，文件和设备的偏移量也是
对齐的（读写对齐便可直接满足条件）
只需要在O_DIRECT情况下，使用mmap申请空间，且读写字节数对齐
而对非O_DIRECT情况下，可使用malloc，字节数不对齐也行，但本例子是对齐的

线程思路:主线程获取随机数写前4k块，将随机数告知其他线程然后等待，其他线程根据
线程数循环获取块来写，直到全部写完（写的部分是互斥的），唤醒主线程，主线程通知检验线程检验，
主线程再获取随机数从头开始写，若边写边校验，则write几次校验一次，
若写完一次再校验，则等主线程收到signal且写线程全部停止后再唤醒校验线程
校验线程校验完成，通知主线程，主线程进行下一次循环	
问题：
1.多线程下的初次检验可能不成功，因为多个线程写，在获取块后写可能先写后续位置，而此时关闭程序
信号处理函数可能将此位置记录于配置文件，而之前的位置可能还没有写完，所以多线程检验目前只
支持在运行过程中检验的两种粒度。

2.后台运行终止时不要用kill -9，无法有信号处理函数，从而无法利用atexit写配置文件，要用
SIGINT 2 SIGTERM 15，即 kill -2 pid
*/
