#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<iostream>
#include<sys/epoll.h>
#include"mysqlpool.h"
#include"addjson.h"

#define BUFFER_SIZE 1024
#define CONTENT_SIZE 1024*1024
#define EPOLLNUM 1024
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//

enum op_type{login_t=0,insert_user_t,insert_article_t,insert_group_t,insert_collect_t,insert_comment_t,insert_user_rel_t,
             query_user_t,query_user_list_t,query_user_rel_t,query_user_rel_list_t,query_group_t,query_group_list_t,
             query_article_t,query_article_list_t,query_comment_t,query_comment_list_t,query_collect_t,query_collect_list_t,
             modify_user_t,modify_article_t,modify_group_t,modify_collect_t,modify_user_rel_t,modify_comment_t,
             delete_user_t,delete_user_rel_t,delete_group_t,delete_article_t,delete_article_list_t,delete_comment_t,delete_comment_list_t,delete_collect_t,delete_collect_list_t,
             };
//type还要加一些如login

enum Status{RES_SUCCESS=0,RES_FAILURE,RES_CONNECTED,RES_DISCONNECTED,RES_PERMISSION_DENEY,REQUEST,RESPONSE};
/*
void add_epoll_fd(int client_fd)
{
    event.data.fd=clientfd;
    event.events=EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&event);//添加到epoll
    //http 连接
    printf("http connect success\n");
}
*/
class thread
{
public:
    void thread_loop(void* arg);
    void do_other_thing();//把callback中的函数指针全部执行
private:
    void* callback(void* arg);//回调
    epoll_event events[EPOLLNUM];
    int epollfd;
    int wake_fd;//在epollfd中注册一个唤醒fd，用于执行完do_other_thing()后唤醒fd
};

int setnonblocking(int fd)
{
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}
void addfd(int epollfd,int fd)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN|EPOLLET;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnonblocking(fd);
}
struct Result
{
    char* json;
    char* protocol;
};
Result* packet_response(void *content,int status,int type)
{
    //返回肯定是12+strlen（json）,json中没有\0
    char* str=NULL;
    char* result=new char[12];
    Result *res=new Result;
    //将结果打包
    if(content!=NULL)
        switch(type)
        {
            case insert_user_t:
                str=struct2json(content,USER);
                break;
            case query_user_t:
                str=struct2json(content,USER);
                break;
            case modify_user_t:
                str=struct2json(content,USER);
                break;
            case delete_user_t:
                str=struct2json(content,USER);
                break;
        }
    res->json=str;
    res->protocol=result;
    int CRC=4;//CRC校验，包括json+4个字段,暂无
    unsigned int length=12;
    if(str!=NULL)
        length+=strlen(str);//包总长度
    memcpy(result,&length,4);
    memcpy(result+4,&type,2);
    memcpy(result+6,&status,2);
    memcpy(result+8,&CRC,4);

    //增加64位前缀，用协议层封装
    return res;
}

char* get_request(int fd)//获取请求并处理,不读到12不返回的
{
    char* buffer=new char[1000];
    buffer[0]='\0';
    int idx=-1;
    int has_read=0;
    int length=12;
    while(has_read<length)
    {
        int n=read(fd,buffer+has_read,length-has_read);
        has_read+=n;
        if(has_read>=12&&idx==-1)
        {
            //获取长度
            idx=1;
            length=trans4((buffer));
        }
    }
    if(length-12==0)
        buffer[12]='\0';
    return buffer;
}

//处理请求
//buffer表示传来的请求，
struct Result* process_request(char* buffer)
{
    int length=trans4((buffer));//len-12==0??????buffer[len-12]=='\0'??
    int status=trans2((buffer+4));
    int type=trans2((buffer+6));
    unsigned int CRC=trans4((buffer));
    char* json=buffer+12;
    //int ret=assert_crc(buffer,CRC);crc检验
    struct Result* response=NULL;
    int state=0;
    if(status==REQUEST)
        switch(type)
        {
            case insert_user_t:
            {
                User* user=(User*)json2struct(json,USER);
                state=insert_user(user);
                response=packet_response(NULL,state,type);
                break;
            }
            case query_user_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER);
                int id=user->user_id;
                user=query_user(id);//查询然后strcut2json 然后协议
                if(user!=NULL)
                    state=RES_SUCCESS;
                response=packet_response(user,state,type);
                break;
            }
            case modify_user_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER);
                state=modify_user(user);//查询然后strcut2json 然后协议
                response=packet_response(NULL,state,type);
                break;
            }
            case delete_user_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER);
                state=delete_user(user->user_id);
                response=packet_response(NULL,state,type);
            }
            default:
                break;
        }
    else
    {
        return NULL;
    }
    return response;
}

int send_response(int fd,struct Result *result)
{
    int index=0;
    int length=0;
    if(result->protocol!=NULL)
        while(index!=12)
        {
            index+=write(fd,result->protocol+index,12-index);
            if(index<0)
                return -1;
        }
    if(result->json!=NULL)
    {
        index=0;
        length=strlen(result->json);
        while(index!=length)
        {
            index+=write(fd,result->json+index,length-index);
            if(index<0)
                return -1;
        }
    }
    return length+12;
}


void work_thread_loop(void* arg)
{
    int epollfd=0;
    epollfd=epoll_create(10);
    epoll_event events[EPOLLNUM];
    char* request=NULL;
    struct Result* response=NULL;
    int len=0;
    while(1)
    {
        int n=epoll_wait(epollfd,events,EPOLLNUM,-1);
        {
            //
            for(int i=0;i<n;i++)
            {
                if(events[i].events|EPOLLIN)//请求到达
                {
                    request=get_request(events[i].data.fd);
                    response=process_request(request);
                    send_response(events[i].data.fd,response);
                }
                if(events[i].events|EPOLLOUT)//有数据发送
                {
                    std::cout<<"some thing send out"<<endl;
                }
                if(events[i].events|EPOLLERR)
                {
                    close(events[i].data.fd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,events[i].data.fd,NULL);
                }
                if(events[i].events==EPOLLRDHUP)//对端关闭连接
                {
                    close(events[i].data.fd);
                    epoll_ctl(epollfd,EPOLL_CTL_DEL,events[i].data.fd,NULL);
                }
            }
        }
        //do_other_thing();
    }
}


int main(int argc,char* argv[])
{
    int serv_fd=0;
    int epollfd=0;
    int thread_num=0;
    int port=0;
    if(argc<=3)
    {
        perror("usage:./filename ip port thread_num\n");
        return 1;
    }
    char* content=(char*)malloc(CONTENT_SIZE);
    char* ip=argv[1];
    port=atoi(argv[2]);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);
    thread_num=atoi(argv[3]);
    int ret=inet_pton(AF_INET,ip,&serv_addr.sin_addr);
    assert(ret>=0);
    
    serv_fd=socket(PF_INET,SOCK_STREAM,0);
    printf("%d\n",serv_fd);
    ret=bind(serv_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    assert(ret!=-1);
    
    ret=listen(serv_fd,5);
    printf("111111111\n");
    
    assert(ret!=-1);
    
    epollfd=epoll_create(10);
    struct epoll_event events[EPOLLNUM];
    struct epoll_event event;
    event.data.fd=serv_fd;
    event.events=EPOLLIN;
    
    sockaddr_in clientaddr;
    socklen_t clientlen=sizeof(clientaddr);
    int clientfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,serv_fd,&event);
    while(1)
    {
        int n=epoll_wait(epollfd,events,EPOLLNUM,-1);
        if(ret<0)
        {
            printf("epoll failure\n");
            break;
        }
        for(int i=0;i<n;++i)
        {
            if(events[i].events|EPOLLIN&&events[i].data.fd==serv_fd)
            {
                //连接到来
                clientfd=accept(serv_fd,(struct sockaddr*)&clientaddr,&clientlen);
                //将fd添加到其他线程的epoll上

                event.data.fd=clientfd;
                event.events=EPOLLIN;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&event);//添加到epoll
                //http 连接
                printf("http connect success\n");
            }
/*
            else if(events[i].events|EPOLLIN)//客户端请求到达
            {
                process_http(epollfd,events[i].data.fd);
                //Sendhttp(epollfd,events[i].data.fd);
            }
            else if(events[i].events|EPOLLRDHUP)//对端至少关闭写连接
            {
                epoll_ctl(epollfd,EPOLL_CTL_DEL,events[i].data.fd,0);
                close(events[i].data.fd);
            }
*/
        }
    }
    return 0;
}

//为json内容添加协议前缀
//最终黑盒数据
/*
                       4字节位
|                                                  |
------------ -------------- ------------ -----------
                        长度
          状态                        操作类型
                        CRC(针对json+上述4项)                        


*/


//get_black_data,第二个参数也可以是一个channel，能够识别那条通路
//在协议中可能存在\0,所以协议部分单独发送，内容跟在后面