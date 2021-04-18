#ifndef FILE_LOCK_H
#define FILE_LOCK_H
#include<errno.h>
#include<fcntl.h>
#include<stdio.h>
//文件锁
int file_lock(int *lck);
#endif