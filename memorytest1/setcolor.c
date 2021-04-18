#include"setcolor.h"
//设置错误显示颜色，参数：color 取red、yellow、bule、green、purple、其他
#ifndef WIN32
int setcolor(char* color)
{
	if(color==NULL)
		return 0;
	int col = 0;
	if (!strcasecmp(color, "red"))
		col = 1;
	else if (!strcasecmp(color, "yellow"))
		col = 2;
	else if (!strcasecmp(color, "blue"))
		col = 3;
	else if (!strcasecmp(color, "green"))
		col = 4;
	else if (!strcasecmp(color, "purple"))
		col = 5;
	else
		col = 0;
	return col;
}
#else
int setcolor(char* color)
{
	int col = 0;
	if (!stricmp(color, "red"))
		col = 1;
	else if (!stricmp(color, "yellow"))
		col = 2;
	else if (!stricmp(color, "blue"))
		col = 3;
	else if (!stricmp(color, "green"))
		col = 4;
	else if (!stricmp(color, "purple"))
		col = 5;
	else
		col = 0;
	return col;
}
#endif