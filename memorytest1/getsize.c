#include"getsize.h"
//获取输入文件大小：2GB->1024*1024*1024*2
long getsize(char* size)
{
	int size1 = 0;
	if (size == NULL)
	{
		return 0;
	}
	else if (size[0] <= '9' && size[0] >= '0')
	{
		int y = 0;
		int i = 0;
		while (size[i] >= '0' && size[i] <= '9')
		{
			y = y * 10 + (size[i] - '0');
			i++;
		}
		if(size[i]=='\0'||size[i]==' ')
			return y;
		if (size[i] == 'G' && (strlen(size + i) <= 1 || size[i + 1] == 'B'))
			size1 = y * 1024 * 1024 * 1024;
		else if (size[i] == 'g' && (strlen(size + i) <= 1 || size[i + 1] == 'b'))
			size1 = y * 1024 * 1024 * 1024;
		else if (size[i] == 'M' && (strlen(size + i) <= 1 || size[i + 1] == 'B'))
			size1 = y * 1024 * 1024;
		else if (size[i] == 'm' && (strlen(size + i) <= 1 || size[i + 1] == 'b'))
			size1 = y * 1024 * 1024;
		else if (size[i] == 'K' && (strlen(size + i) <= 1 || size[i + 1] == 'B'))
			size1 = y * 1024;
		else if (size[i] == 'k' && (strlen(size + i) <= 1 || size[i + 1] == 'b'))
			size1 = y * 1024;
		else if (size[i] == 'B' && strlen(size + i) == 1)
			size1 = y;
		else if (size[i] == 'b' && strlen(size + i) == 1)
			size1 = y;
		else
			size1 = 0;
	}
	else
	{
		return 0;
	}
	return size1;
}
