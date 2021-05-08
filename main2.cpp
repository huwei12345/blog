
//先把一个简单的协议完成，能够和前端通信，然后再上层

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
             test};
//type还要加一些如login

#define BUFFER_SIZE 1024
#define CONTENT_SIZE 1024*1024
#define EPOLLNUM 1024


#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
char buf[BUFFER_SIZE];
int len=0;//当前读到buf长度
int length=12;//总请求长度
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
Result* packet_response(char *str,int status,int type)
{
    //返回肯定是12+strlen（json）,json中没有\0
    char* result=new char[12];
    Result *res=new Result;
    //将结果打包
    if(status==FAILURE||status==INSERT_ERROR||status==GET_MYSQL_ERROR||status==MODIFY_ERROR)
    {
        //错误，无数据
        res->json=NULL;
        unsigned int length=12;
        if(str!=NULL)
            length+=strlen(str);//包总长度
        memcpy(result,&length,4);
        memcpy(result+4,&type,2);
        memcpy(result+6,&status,2);
        int CRC=0;
        memcpy(result+8,&CRC,4);
        res->protocol=result;
        printf("in packet\n");
    }
    else
    {
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
    }
    return res;
}

//处理请求
//buffer表示传来的请求，
struct Result* process_request(char* buffer)
{
    /*
    Result *result=new Result;
    result->json=new char[20];
    memset(result->json,'\0',20);
    strcpy(result->json,"hello world");
    result->protocol=new char[12];
    int x=1;
    memcpy(result->protocol,&x,4);
    memcpy(result->protocol+4,&x,4);
    memcpy(result->protocol+8,&x,4);
    return result;
    */
    int length=trans4((buffer));//len-12==0??????buffer[len-12]=='\0'??
    printf("length=%d\n",length);
    int status=trans2((buffer+4));
    printf("status=%d\n",status);
    int type=trans2((buffer+6));
    printf("type=%d\n",type);
    unsigned int CRC=trans4((buffer));
    printf("CRC=%d\n",CRC);
    char* json=buffer+12;
    printf("json=%s\n",json);
    //int ret=assert_crc(buffer,CRC);crc检验
    struct Result* response=NULL;
    int state=0;
    char* str=NULL;
    if(status==REQUEST)
        switch(type)
        {
            case login_t:
            {
                printf("json=%s\n",json);
                User* user=(User*)json2struct(json,USER);
                User* user_new=NULL;
                printf("account=%s,password=%s",user->account,user->password);
                user_new=query_my_user(user->account,user->password);
                //printf("state=%d\n",state);
                if(user_new!=NULL)
                    state=SUCCESS;
                printf("222222222222\n");
                str=struct2json(user_new,USER);
                response=packet_response(str,state,type);
                break;
            }
            case insert_user_t:
            {
                printf("json=%s\n",json);
                User* user=(User*)json2struct(json,USER);
                state=insert_user(user);
                printf("state=%d\n",state);
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
                    state=SUCCESS;
                response=packet_response(str,state,type);
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
        response=packet_response(NULL,FAILURE,0);
    }
    return response;
}

int send_response(int fd,struct Result *result)
{
    printf("11111111111111\n");
    int index=0;
    int length_t=0;
    if(result->protocol!=NULL)
        while(length_t!=12)
        {
            index=write(fd,result->protocol+length_t,12-length_t);
            printf("send %d bytes\n",index);
            if(index<0)
            {
                printf("send error,index=%d,errno=%d\n",index,errno);
                return -1;
            }
            else if(index==0)
            {
                printf("connect down\n");
                return -1;
            }
            length_t+=index;
        }
    if(result->json!=NULL)
    {
        index=0;
        int len_t=0;
        length_t=strlen(result->json);
        while(index!=length_t)
        {
            index=write(fd,result->json+len_t,length_t-len_t);
            printf("send %d bytes\n",index);
            if(index<0)
                return -1;
            else if(index==0)
            {
                printf("connect down\n");
                return -1;
            }
            len_t+=index;
        }
    }
    len=0;
    length=12;
    return length_t+12;
}


void* work_thread_loop(void* arg)
{
    int epollfd=0;
    epollfd=epoll_create(10);
    epoll_event events[EPOLLNUM];
    char* request=NULL;
    struct Result* response=NULL;
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
                    std::cout<<"some thing send out"<<std::endl;
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

struct arg
{
    ;
};


/*非阻塞*/
int setnoblocking(int fd)
{
    int old_option =fcntl(fd,F_GETFL);
    int new_option = old_option|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}
void addfd(int epollfd,int fd,bool enable_et)
{
    epoll_event event;
    event.data.fd=fd;
    event.events=EPOLLIN;
    if(enable_et)
    {
        event.events|=EPOLLET;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    setnoblocking(fd);
}
void lt(epoll_event* events,int number,int epollfd,int listenfd)
{
    char buf[BUFFER_SIZE];
    for(int i=0;i<number;i++)
    {
        int sockfd=events[i].data.fd;
        if(sockfd==listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength=sizeof(client_address);
            int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);
            addfd(epollfd,connfd,false);
        }
        else if(events[i].events&EPOLLIN)
        {
            printf("event trigger once\n");
            memset(buf,'\0',BUFFER_SIZE);
            int ret=recv(sockfd,buf,BUFFER_SIZE-1,0);
            if(ret<=0)
            {
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n",ret,buf);
        }
        else
        {
            printf("something else happened\n");
        }
    }
}

void et(epoll_event* events,int number,int epollfd,int listenfd)
{
    for(int i=0;i<number;i++)
    {
        int sockfd=events[i].data.fd;
        if(sockfd==listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addresslength=sizeof(client_address);
            int connfd=accept(listenfd,(struct sockaddr*)&client_address,&client_addresslength);
            addfd(epollfd,connfd,true);
        }
        else if(events[i].events&EPOLLIN)
        {
            printf("event trigger once\n");
            while(len<length)
            {
                int ret=recv(sockfd,buf+len,BUFFER_SIZE-1,0);
                printf("ret=%d\n",ret);
				if(ret<0)
                {
                    //表示已全读完
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                    {
                        printf("read later\n");
                        break;
                    }
                    printf("net error\n");
                    close(sockfd);
                    break;
                }
                else if(ret==0)
                {
                    printf("connect down\n");
                    close(sockfd);
                }
                else
                {
                    len+=ret;
                    if(len>=4)
                        length=trans4(buf);
                    printf("length=%d",length);
                    if(len>=length)
                    {
                        Result *x=process_request(buf);
                        send_response(sockfd,x);
                    }
                }
            }
        }
        else
        {
            printf("something else happened \n");
        }
    }
}
int main(int argc,char* argv[]){
    if(argc<=2)
    {
        printf("usage: %s ip_address port_number\n",basename(argv[0]));
        return 1;
    }
    memset(buf,'\0',BUFFER_SIZE);
    const char* ip=argv[1];
    int port=atoi(argv[2]);
    int ret=0;
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port=htons(port);
    int listenfd=socket(PF_INET,SOCK_STREAM,0);
    assert(listenfd>=0);
    ret=bind(listenfd,(struct sockaddr*)&address,sizeof(address));
    assert(ret!=-1);
    ret=listen(listenfd,5);
    assert(ret!=-1);
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd=epoll_create(5);
    assert(epollfd!=-1);
    addfd(epollfd,listenfd,true);
    mysql_init();
    while(1)
    {
        int ret=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if(ret<0)
        {
            printf("epoll failure\n");
            break;
        }
        //lt(events,ret,epollfd,listenfd);
        et(events,ret,epollfd,listenfd);
    }
    close(listenfd);
    return 0;
}
