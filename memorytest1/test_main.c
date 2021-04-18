#include "memorytest.h"
#include "xtest.h"
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#define trans4(x) (x[0])&(0x000000ff)|(x[1]<<8)&(0x0000ff00)|(x[2]<<16)&(0x00ff0000)|(x[3]<<24)&(0xff000000)//�ַ���int������


/*可单测的函数
long getsize(char* size);//�ļ���Сת��������2GB->1024*1024*1024*2
int setcolor(char* color);//���ô�����ʾcolor
void printferr(long offset, int correctnum, int errornum, int color, int daemonon);//�����һ�´���
int Read(int fd, char* buffer, int size);//��ʹ��дָ�벻�ı��read
void* Malloc(size_t size, int openD);//����O_DIRECT���ļ�
int file_lock(int *lck);
void buf_write_num(int xtemp, char* tmpptr, int blocksize);//��num���ְ�intд��buf��
*/
//完成使用场景的测试
int SetcolorTest(char* str)
{
 //   printf("22222\n");
    return setcolor(str);
}
TEST(test, scene)
{
    ASSERT_EQ(SetcolorTest(""),0);
    ASSERT_EQ(SetcolorTest("     "),0);
    ASSERT_EQ(SetcolorTest("red"),1);
    ASSERT_EQ(SetcolorTest("yellow"),2);
    ASSERT_EQ(SetcolorTest("blue"),3);
    ASSERT_EQ(SetcolorTest("green"),4);
    ASSERT_EQ(SetcolorTest("purple"),5);
    ASSERT_EQ(SetcolorTest("brown"),0);
    ASSERT_EQ(SetcolorTest(NULL),0);
    
    ASSERT_EQ(SetcolorTest("11111"),0);
    ASSERT_EQ(SetcolorTest("12312\n\nc\t"),0);
    ASSERT_EQ(SetcolorTest("gjfjhfgfjfjhgjfjhfggfjhfhjcghdfjfjfjhjg"),0);
}

//  测试键值对存在的情况
TEST(printferr, exist)
{
    printferr(1,1,1,0,0);
    printferr(1,1,1,1,0);
    printferr(1,1,1,2,0);
    printferr(1,1,1,3,0);
    printferr(1,1,1,4,0);
    printferr(1,1,1,5,0);
    printferr(1,1,1,5,1);
    printferr(1,1,1,0,1);
}
int Readtest()
{
    int fd=open("1.txt",O_CREAT|O_RDWR,0666);
    char *buf=malloc(128);
    memset(buf,0,128);
    write(fd,buf,128);
    write(fd,buf,128);
    write(fd,buf,128);
    lseek(fd,128,SEEK_SET);
    memset(buf,-1,128);
    Read(fd,buf,128);
    for(int i=0;i<128;i++)
        printf("%c",buf[i]+'0');
    printf("\n");
    int y=lseek(fd,0,SEEK_CUR);
    close(fd);
    return y;
}

TEST(Read,seekcorrect)
{
    ASSERT_EQ(Readtest(),128);  
}


TEST(Malloc, success)
{
    ASSERT_TRUE(Malloc(0,1)==NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(0,0)==NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(1024,1)!=NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(512,1)!=NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(4096,1)!=NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(1024,0)!=NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(10,0)!=NULL);
//  printf("11\n");
    ASSERT_TRUE(Malloc(10,1)!=NULL);
//  printf("11\n");
}

TEST(getsize, success)
{
    ASSERT_TRUE(getsize("2G")==2*1024*1024*1024);
    ASSERT_TRUE(getsize("2GB")==2*1024*1024*1024);
    ASSERT_TRUE(getsize("2gb")==2*1024*1024*1024);
    ASSERT_TRUE(getsize("2g")==2*1024*1024*1024);
    ASSERT_TRUE(getsize("2GB")==2*1024*1024*1024);    
    ASSERT_TRUE(getsize("8G")==8*1024*1024*1024);
    ASSERT_TRUE(getsize("8GB")==8*1024*1024*1024);
    ASSERT_TRUE(getsize("8gb")==8*1024*1024*1024);
    ASSERT_TRUE(getsize("8g")==8*1024*1024*1024);
    ASSERT_TRUE(getsize("8GB")==8*1024*1024*1024);    
    ASSERT_TRUE(getsize("")==0);
    ASSERT_TRUE(getsize(NULL)==0);
    ASSERT_TRUE(getsize("1")==1);
    ASSERT_TRUE(getsize("2")==2);
    ASSERT_TRUE(getsize("512")==512);
    ASSERT_TRUE(getsize("1024")==1024);
    ASSERT_TRUE(getsize("1")==1);
    ASSERT_TRUE(getsize("1B")==1);
    ASSERT_TRUE(getsize("1b")==1);
    ASSERT_EQ(getsize("1K"),1024);
    ASSERT_EQ(getsize("1KB"),1024);
    ASSERT_EQ(getsize("1kb"),1024);
    ASSERT_EQ(getsize("1k"),1024);
    ASSERT_EQ(getsize("1M"),1024*1024);
    ASSERT_EQ(getsize("1MB"),1024*1024); 
    ASSERT_EQ(getsize("1m"),1024*1024);
    ASSERT_EQ(getsize("1mb"),1024*1024);
    ASSERT_EQ(getsize("1T"),0);
}



TEST(buf_write_num,ssucces)
{
    char* buf=malloc(2048);
    int xtemp=1;
    int blocksize=2048;
    ASSERT_TRUE(buf_write_num(1,NULL,128,SECTOR_SIZE)==0);
    ASSERT_TRUE(buf_write_num(1,buf,0,SECTOR_SIZE)==0);
    ASSERT_TRUE(buf_write_num(1,buf,128,SECTOR_SIZE)==0);
    ASSERT_TRUE(buf_write_num(1,buf,512,SECTOR_SIZE)==1);
    ASSERT_TRUE(buf_write_num(1,buf,1024,SECTOR_SIZE)==1);
    ASSERT_TRUE(buf_write_num(1,buf,1536,SECTOR_SIZE)==1);
    ASSERT_TRUE(buf_write_num(1,buf,2048,SECTOR_SIZE)==1);
}

int verify(int blocksize)
{
    char* buf=malloc(2048);
    int temp=0;
    buf_write_num(1,buf,blocksize,SECTOR_SIZE);
    for(int i=0;i<blocksize;i++)
    {
        if(trans4((buf+i))==1)
            continue;
        else
        {
            temp=1;
            break;
        }
    }
    free(buf);
    if(temp==0)
        return 1;
    else
        return 0;
}
TEST(buf_write_num,writecorrect)
{
    ASSERT_TRUE(verify(1024)==0);
    ASSERT_TRUE(verify(2048)==0);
    ASSERT_TRUE(verify(1536)==0);
}
TEST(file_lock,success)
{
    int lck;
    ASSERT_TRUE(file_lock(NULL)==0);

    ASSERT_TRUE(file_lock(&lck)==1);
    ASSERT_TRUE(file_lock(&lck)==0);
    close(lck);
    remove("a.lock");
}
int main(int argc, char **argv)
{
	return xtest_start_test(argc, argv);
}
