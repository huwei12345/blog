#ifndef WIN_LINUX_H
#define WIN_LINUX_H
#include<stdio.h>
#ifndef WIN32
#include<unistd.h>
#include<pthread.h>
#include <sys/mman.h>
#define __USE_GNU 1
#else
#include<windows.h>
#include<io.h>
#include<process.h>
#endif
#include<fcntl.h>

#ifdef WIN32
typedef CRITICAL_SECTION Mutex;
typedef CONDITION_VARIABLE Cond;
typedef void (*Thread_func)(void*);
#else
typedef pthread_cond_t Cond;
typedef pthread_mutex_t Mutex;
typedef void* (*Thread_func)(void*);
#endif
//多线程windows兼容封装
void Mutex_Init(Mutex* mutex);
void Cond_Init(Cond* cond);
void Mutexlock(Mutex* mutex);
void Mutexunlock(Mutex* mutex);
void Cond_Wait(Cond* cond, Mutex* mutex);
void Cond_Signal(Cond* cond);
void Cond_SignalAll(Cond* cond);
int Pthread_create(Thread_func);
#endif