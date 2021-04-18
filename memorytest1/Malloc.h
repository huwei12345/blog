#ifndef MY_MALLOC_H
#define MY_MALLOC_H
#include<stdio.h>
#include <sys/mman.h>
#include<stdlib.h>
#include<errno.h>
void* Malloc(size_t size, int openD);//����O_DIRECT���ļ�
#endif