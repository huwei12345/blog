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
enum op_type{insert_user,insert_article,insert_group,insert_collect,insert_comment,insert_user_rel,
             query_user,query_user_list,query_user_rel,query_user_rel_list,query_group,query_group_list,
             query_article,query_article_list,query_comment,query_comment_list,query_collect,query_collect_list,
             modify_user,modify_article,modify_group,modify_collect,modify_user_rel,modify_comment,
             delete_user,delete_user_rel,delete_group,delete_article,delete_article_list,delete_comment,delete_comment_list,delete_collect,delete_collect_list};
void add_epoll_fd(int client_fd)
{
    event.data.fd=clientfd;
    event.events=EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&event);//添加到epoll
    //http 连接
    printf("http connect success\n");
}

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
void set_nonblocking(int fd)
{

}

void get_request(int fd)//获取请求并处理
{
    char* buffer;
    int idx=-1;
    int has_read=0;
    int length=16;
    int status=0;
    int op_type=0;
    int CRC=0;
    while(has_read<length)
    {
        if(idx==0)
            break;
        int n=read(fd,buffer+has_read,BUFFER_SIZE);
        has_read+=n;
        if(n>=16&&idx==-1)
        {
            //获取长度
            length=getint(buffer,4);
            status=getint(buffer+4,2);
            op_type=getint(buffer+6,2);
            CRC=getint(buffer+8,4);
        }
    }
    char* json=buffer+16;
    int ret=assert_crc(buffer,CRC);
    if(ret==0)
        return FAILURE;
    if(status==REQUEST)
        switch(op_type)
        {
            case insert_user:
            {
                User* user=json2struct(json);
                int state=insert_user();
                char* response=packet_response(state);
                send_response(response);
                break;
            }
            case query_user:
            {
                User* user=new User;
                user=json2struct(json);
                int id=user->id;
                user=query_user(id);//查询然后strcut2json 然后协议
                char* json_r=struct2json(user);
                char* result=addpro(json_r);
                send_response(result);
                break;
            }
            case modify_user:
            {
                User* user=new User;
                user=json2struct(json);
                int res=modify_user(user);//查询然后strcut2json 然后协议
                //不需要传json只传16字节结果就行
                char* json_r=NULL;
                char* result=addpro(json_r);
                send_response(result);
                break;
            
            }
            case delete_user:
            {
                User* user=new User;
                user=json2struct(json);
                int res=delete_user(user->id);
                char* json_r=NULL;
                char* result=addpro(json_r);
                send_response(result);
            }
            default:
        }
    else
    {
        return FAILURE;
    }
}
void work_thread_loop(void* arg)
{
    int epollfd=0;
    epollfd=epoll_create(10);
    epoll_event events[EPOLLNUM];
    while(1)
    {
        int n=epoll_wait(epollfd,events,EPOLLNUM,-1);
        {
            //
            for(int i=0;i<n;i++)
            {
                if(events[i].events==EPOLLIN)//请求到达
                {
                    get_request(events[i].data.fd);
                }
                if(events[i].events==EPOLLOUT)//有数据发送
                {
                    send_response(events[i].data.fd);
                }
                if(events[i].events==EPOLLERR)
                {

                }
                if(events[i].events==EPOLLRDHUP)//对端关闭连接
                {

                }
            }
        }
        do_other_thing();
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
