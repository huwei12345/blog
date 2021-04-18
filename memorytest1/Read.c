#include"Read.h"
//无偏移影响读取文件到缓存区
int Read(int fd, char* buffer, int size)//��Ӱ��ָ���ȡ
{
	lseek(fd, -1 * size, SEEK_CUR);
	int ret = 0;
	int least = size;
	while (ret != size)
	{
		if (ret != -1)
		{
			ret += read(fd, buffer+ret, least);
			least = size-ret;
		}
	}
	return ret;
}