#include"file_lock.h"
//文件锁
int file_lock(int* lck)
{
	if(lck==NULL)
	{
		printf("file_fd_error\n");
		return 0;
	}
	//�ļ���
	*lck = open("a.lock", O_RDONLY | O_CREAT | O_EXCL | O_TRUNC, 0666);
	if (*lck < 0)
	{
		if (errno == EEXIST)
		{
			printf("./memorytest is runing and can't open again\n");
			return 0;
		}
		else
		{
			printf("file_lock_error,errno=%d\n",errno);
			return 0;
		}
	}
	return 1;
}