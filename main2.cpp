
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

#define BUFFER_SIZE 1024
#define CONTENT_SIZE 1024*1024
#define EPOLLNUM 1024
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//
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

//处理请求
//buffer表示传来的请求，
struct Result* process_request(char* buffer)
{
    Result *result=new Result;
    result->json=new char[10];
    strcpy(result->json,"hello world");
    result->protocol=new char[12];
    int x=1;
    memcpy(result,&x,4);
    memcpy(result+4,&x,4);
    memcpy(result+8,&x,4);
    return result;
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
