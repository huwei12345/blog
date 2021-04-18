#include"buf_write_num.h"
/**
 * 程序功能：存储系统一致性检验
 * 本文件：存放api函数，可用于单元测试
 * 时间：2021年3月9日20点41分
**/ 
//将num数字按int写到buf中，参数：随机数、缓冲区、大小，无返回值
int buf_write_num(int xtemp, char* tmpptr, int blocksize,int sector_size)
{
	if(tmpptr==NULL||blocksize==0)
	{
		printf("tmpptr is NULL\n");
		return 0;
	}
	if(blocksize%sector_size!=0)
	{
		printf("blocksize %% %d!=0\n,",sector_size);
		return 0;
	}
	int int_size=sizeof(int);
	int m = blocksize,z=0;
	while (m != 1)//判断数字是否是2的n次
	{
		if (m % 2 != 0)
		{
			z = 1;
			break;
		}
		m = m / 2;
	}
	memcpy(tmpptr, &xtemp, int_size);//赋值一个数
	if (z == 1)//例如512*6
	{
		for (int j = int_size; j < sector_size; j *= 2)//赋值一个扇区
		{
			memcpy(tmpptr + j, tmpptr, j);
		}
		for (int j = 1; j < blocksize / sector_size; j++)//赋值一个块
		{
			memcpy(tmpptr + j * sector_size, tmpptr, sector_size);
		}
	}
	else//4096
	{
		for (int i = int_size; i < blocksize; i *= 2)
			memcpy(tmpptr + i, tmpptr, i);//写前4k内容，乘法增长2 4 8 16 32
	}
	return 1;
}
