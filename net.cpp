//function 客户端测试，能接收网络传输的json
//comment 此函数能跑通，qt就能接收数据
//date 2021.5.27
//author @huwei
//other 

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<iostream>
#include<sys/epoll.h>
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//
using namespace std;
enum Status{REQUEST=0,REQ_FORMAT_ERROR,SUCCESS,FAILURE,PERMISSION_DENEY,NET_BROKEN,INSERT_ERROR,GET_MYSQL_ERROR,MODIFY_ERROR,DELETE_ERROR,QUERY_ERROR};
int setnoblocking(int fd)
{
    int old_option =fcntl(fd,F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}

int main(int argc,char* argv[])
{
	int y=0;
    char* buf=new char[1000];
    memset(buf,'\0',1000);
    char *json=new char[500];
    memset(json,'\0',500);
	short status=1;
    short type=0;//登录，返回User
    memcpy(buf+4,&status,2);//status request
    memcpy(buf+6,&type,2);//type insert_user_t
	
	strcpy(json,"{\"account\": \"huwei123\",\"password\":      \"huwei123\"}");
	json[strlen(json)]='\0';
	int length=strlen(json)+12+1;
    memcpy(buf,&length,4);
    if(argc<=2)
    {
        printf("usage: %s ip_address port_number\n",basename(argv[0]));
        return 1;
    }
    const char* ip=argv[1];
    int port=atoi(argv[2]);

	int clientfd=socket(AF_INET,SOCK_STREAM,0);
	if(clientfd==-1)
	{
		cout<<"create client socket error."<<endl;
		return -1;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family=AF_INET;
	inet_pton(AF_INET,ip,&serveraddr.sin_addr);
    serveraddr.sin_port=htons(port);
	//2.��?ӷ????? 
	if(connect(clientfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==-1)
	{
		cout<<"connect socket error."<<endl;
		return -1;
	}
	//3.???????????????? 
	int ret=send(clientfd,buf,12,0);

    ret=send(clientfd,json,strlen(json)+1,0);
	//std::cout<<"send data successfully,data:"<<std::endl;
	char *recvBuf=new char[1000];
	memset(recvBuf,'\0',1000);
	ret=0;
	length=12;
	int len=0;
	//setnoblocking(clientfd);
	while(ret<length)
	{
		len=recv(clientfd,recvBuf+ret,length-ret,0);
		printf("get %d byte\n",len);
		if(len>0)
			ret+=len;
		else if(len==0)
		{
			close(clientfd);
			break;
		}
		if(ret>=4)
		{
			length=trans4(recvBuf);
			printf("length=%d\n",length);
		}
	}
	if(ret>12)
	{
		printf("ret=%d\n",ret);
		int length=trans4(recvBuf);
		printf("length=%d\n",length);
		if(length>=12)
		{
			int status=trans2((recvBuf+4));
			int type=trans2((recvBuf+6));
			if(status==FAILURE)
				printf("get user error,status=%d\n",status);
			printf("get type = %d\n",type);
		}
		if(length>12)
		{
			printf("json: %s\n",recvBuf+12);
		}
		else
			printf("no josn text\n");
		//输出
	}
	else
	{
		printf("get error,ret=%d\n",ret);
//	std::cout<<"recv data error,data"<<recvBuf<<std::endl;
	}
	//5.?ر?socket 
	close(clientfd);
	return 0; 
}
