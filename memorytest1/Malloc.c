#include"Malloc.h"
//Malloc封装，如果O_DIRECT开启，须使用mmap
void* Malloc(size_t size, int openD)//��O_DIRECT��ʽ�򿪵��ļ���Ӧ�Ļ�����
{
	if(size==0)
		return NULL;
	//���ڱ������ж�д��Ϊ512�ֽڵı���������Ҫ����write����read�Ķ�д�ֽ�������
	if (openD == 0)
		return malloc(size);
	else
	{
#ifndef WIN32
		char* buffer = (char*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (MAP_FAILED == buffer)
		{
			printf("mmap error!errno=%d\n", errno);
			return NULL;
		}
		return buffer;
#else
		return malloc(size);
#endif
	}
}