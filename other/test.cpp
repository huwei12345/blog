#include<iostream>
#include<string.h>
using namespace std;
class A
{
    public:
    A():b(NULL),c(1){}
    char* b;
    int c;
};
int main()
{
    A *a=new A[10];
    A *b=a;
    char *str=new char[1000];
    strcpy(str,"hello");
    for(int i=0;i<10;i++)
    {
        a->c=i;
        a->b=str;
        a++;
    }
    a=b;
    for(int i=0;i<10;i++)
    {
        printf("%d\n",a->c);
        printf("%s\n",a->b);
        a++;
    }
        snprintf(str,100,"hello world");
        snprintf(str+strlen(str),100,"this is my world");
        snprintf(str+strlen(str),100,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        snprintf(str+strlen(str),100,"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    printf("str=%s\n",str);

    char* name=new char[10];
    strcpy(name,"男");
    for(int i=0;i<strlen(name);i++)
    {
        printf("%c ",name[i]);
    }
    return 0;
}