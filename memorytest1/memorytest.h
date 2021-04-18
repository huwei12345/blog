#ifndef MEMORY_TEST_H
#define MEMORY_TEST_H
#include"wintolinux.h"

#include"file_lock.h"
#include"setcolor.h"
#include"printferr.h"
#include"Read.h"
#include"Malloc.h"
#include"getsize.h"
#include"buf_write_num.h"
#ifndef WIN32
#include<unistd.h>
#else
#include"getopt.h"
#endif
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<signal.h>
#include<errno.h>
#include<assert.h>


#define FIRSTBLOCK_NUM 1024*4 //��һ�����С,4K
#define SECTOR_SIZE 512 //������С
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//�ַ���int������

//线程函数
#ifndef WIN32
void* hd_write_verify_thread(void* args);//�����̺߳���
void* hd_write_thread(void* args);//д�߳�
#else
void hd_write_verify_thread(void* args);//�����̺߳���
void hd_write_thread(void* args);//д�߳�
#endif

//添加信号，在后台运行时，接收SIGUSER1,可显示实时数据
void addsig(int sig);//�����ź�
void sig_handler(int sig);//�źŴ�������
//后台运行
#ifndef WIN32
int daemon_init(int* ttyfd, int* lck);
#endif
//信号相关，关闭程序执行server_on_exit
void signal_crash_handler(int sig);
void signal_exit_handler(int sig);
void server_on_exit(void);
//业务代码
void first_verify();//����ʱ��һ����֤
int getconfig(int *writen,int *randconfig,long *offsetconfig,int *blocksizeconfig,int *threadnum);
void first_write(char* memptr);//д4K��ĺ���
int my_getopt(int argc, char** argv, char* version, int *threadnum);
void thread_init();
void Daemon_init();
int Open_trunc(char* filename,int openD,int* fd);
int Open(char* filename, int openD, int* fd);
void mainfunc(char* memptr);
void verify_after_finish();//写完后随机校验
void verify_after_write();//写的过程中随机校验
void Printhelp();

#ifndef WIN32
pthread_t pid;
#endif

#endif