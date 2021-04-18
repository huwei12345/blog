#include"printferr.h"
//输出错误，带颜色，带后台运行写入日志文件
void printferr(long offset, int correctnum, int errornum, int color, int daemonon)
{
	char str[100];
	if (daemonon == 0)
		switch (color)
		{
		case 1:
			printf("\033[1merror in offset \033[31m%ld\033[0m\033[1m,correct num:%d,false num:\033[31m%d\033[0m\n", offset, correctnum, errornum);
			break;
		case 2:
			printf("\033[1merror in offset \033[33m%ld\033[0m\033[1m,correct num:%d,false num:\033[33m%d\033[0m\n", offset, correctnum, errornum);
			break;
		case 3:
			printf("\033[1merror in offset \033[34m%ld\033[0m\033[1m,correct num:%d,false num:\033[34m%d\033[0m\n", offset, correctnum, errornum);
			break;
		case 4:
			printf("\033[1merror in offset \033[32m%ld\033[0m\033[1m,correct num:%d,false num:\033[32m%d\033[0m\n", offset, correctnum, errornum);
			break;
		case 5:
			printf("\033[1merror in offset \033[35m%ld\033[0m\033[1m,correct num:%d,false num:\033[35m%d\033[0m\n", offset, correctnum, errornum);
			break;
		default:
			printf("error in offset %ld,correct num:%d,false num:%d\n", offset, correctnum, errornum);
			break;
		}
	else
	{
		sprintf(str, "error in offset %ld,correct num:%d,false num:%d\n", offset, correctnum, errornum);
		write(1, str, strlen(str));
	}
}