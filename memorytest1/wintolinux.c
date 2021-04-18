#include"wintolinux.h"
#ifndef WIN32
extern pthread_t pid;
#endif

void Cond_Wait(Cond* cond, Mutex* mutex)
{
#ifdef WIN32
	SleepConditionVariableCS(cond, mutex, INFINITE);//�ȴ�����д�߳�д��
#else
	pthread_cond_wait(cond, mutex);
#endif
}

void Cond_Signal(Cond* cond)
{
#ifdef WIN32
	WakeConditionVariable(cond);
#else
	pthread_cond_signal(cond);
#endif
}

void Cond_SignalAll(Cond* cond)
{
#ifdef WIN32
	WakeAllConditionVariable(cond);//֪ͨ�����߳�
#else
	pthread_cond_broadcast(cond);//֪ͨ�����߳�
#endif
}

void Mutexunlock(Mutex* mutex)
{
#ifdef WIN32
	LeaveCriticalSection(mutex);
#else
	pthread_mutex_unlock(mutex);
#endif
}

void Mutexlock(Mutex* mutex)
{
#ifdef WIN32
	EnterCriticalSection(mutex);
#else
	pthread_mutex_lock(mutex); 
#endif
}

void Mutex_Init(Mutex* mutex)
{
#ifdef WIN32
	InitializeCriticalSection(mutex);
#else
	pthread_mutex_init(mutex, NULL);
#endif
}

void Cond_Init(Cond* cond)
{
#ifdef WIN32
	InitializeConditionVariable(cond);
#else
	pthread_cond_init(cond,NULL);
#endif
}

int Pthread_create(Thread_func thread_func)
{
#ifndef WIN32
	pthread_create(&pid, NULL, thread_func, NULL);//���������߳�
#else
	_beginthread(thread_func, 0, NULL);
#endif
	return 0;
}

int Open_trunc(char* filename,int openD,int *fd)
{
#ifndef WIN32
	if (!openD)//open in O_TRUNC
		*fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);//���ļ�
	else
		*fd = open(filename, O_RDWR | O_DIRECT | O_CREAT | O_TRUNC, 0666);
	//ֱ��IO��Linux����Ӧ�ó�����ִ�д���IOʱ�ƹ����������ٻ��棬
	//���û��ռ�ֱ�ӽ����ݴ��ݵ��ļ�������豸����Ϊֱ��IO��direct IO��������IO��raw IO����
#else
	if (!openD)//open in O_TRUNC
		*fd = open(filename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666);//���ļ�
	else
		*fd = open(filename, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0666);
#endif
	return *fd;
}

int Open(char* filename, int openD, int* fd)
{
#ifndef WIN32
	if (!openD)
		*fd = open(filename, O_RDWR | O_CREAT, 0666);//���ļ�
	else
		*fd = open(filename, O_RDWR | O_DIRECT | O_CREAT, 0666);
	//ֱ��IO��Linux����Ӧ�ó�����ִ�д���IOʱ�ƹ����������ٻ��棬
	//���û��ռ�ֱ�ӽ����ݴ��ݵ��ļ�������豸����Ϊֱ��IO��direct IO��������IO��raw IO����
#else
	if (!openD)
		*fd = open(filename, O_RDWR | O_CREAT | O_BINARY, 0666);//���ļ�
	else
		*fd = open(filename, O_RDWR | O_CREAT | O_BINARY, 0666);
#endif
	return *fd;
}