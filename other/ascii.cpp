#include<iostream>
#include<stdio.h>
#include<string.h>
using namespace std;
int main()
{
    char* name=new char[100];
    strcpy(name,"山西阳泉");
    for(int i=0;i<strlen(name);i++)
    {
        printf("%d\n",name[i]);
    }
    printf("%s",name);
    return 0;
}