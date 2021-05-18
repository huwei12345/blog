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
#include<pthread.h>
#include<sys/epoll.h>
#include<signal.h>
#include<functional>
#include<map>
#include"mysqlpool.h"
#include"addjson.h"

#define BUFFER_SIZE 1024
#define CONTENT_SIZE 1024*1024
#define PROTO_SIZE 12
#define EPOLLNUM 1024
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//

enum op_type{login_t=0,insert_user_t,insert_article_t,insert_group_t,insert_collect_t,insert_comment_t,insert_user_rel_t,
             query_user_t,query_user_list_t,query_user_rel_t,query_user_rel_list_t,query_group_t,query_group_list_t,
             query_article_t,query_article_title_t,query_article_list_t,query_comment_t,query_comment_list_t,query_collect_t,query_collect_list_t,
             modify_user_t,modify_article_t,modify_group_t,modify_collect_t,modify_user_rel_t,modify_comment_t,
             delete_user_t,delete_user_rel_t,delete_group_t,delete_article_t,delete_article_list_t,delete_comment_t,delete_comment_list_t,delete_collect_t,delete_collect_list_t,
             test};

enum failure_type{
    login_f=0,//
    insert_user_error,//
    insert_user_error_2,//

};
//type还要加一些如login

#define EPOLLNUM 1024
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

//每个连接一个
class Buffer
{
    public:
    Buffer():buf(new char[BUFFER_SIZE]),
            length(PROTO_SIZE),
            len(0)
    {
        memset(buf,'\0',BUFFER_SIZE);
    }
    ~Buffer()
    {
        if(buf!=NULL)
            delete buf;
        buf=NULL;
    }
    char* buf;
    int length;
    int len;
};

std::map<int,Buffer*> buffer_map;//每个连接一个Buffer
void* thread_func(void* arg);
void addfd(int epollfd,int fd);
int setnoblocking(int fd);

struct Result
{
    char* json;
    char* protocol;
};
struct Result* process_request(char* buffer);
Result* packet_response(char *str,int status,int type);
int send_response(int fd,struct Result *result);

class thread
{
public:
    int pipe_fd[2];//在epollfd中注册一个唤醒fd，用于执行完do_other_thing()后唤醒fd
    pthread_t pid;
    int epollfd;
    int idx;
};
class threadpool
{
    private:
    //构造函数私有，单例
    threadpool(int listenfd,int thread_number);
    public:
    //单例
    static threadpool* create(int listenfd,int thread_number=8)
    {
        printf("%d %d\n",listenfd,thread_number);
        if(!instance)
            instance=new threadpool(listenfd,thread_number);
        return instance;
    }
    //启动进程池
    void run();
    void run_loop(int idx);
    void init();
    private:
    void setup_sig_pipe();
    ~threadpool()
    {
        delete[] threads;
    }
    private:
    static const int MAX_THREAD_NUMBER=16;
    //每个子进程最多能处理的客户数量
    static const int USER_PER_PROCESS=65536;
    //epoll能最多处理的事件数
    static const int MAX_EVENT_NUMBER1=10000;
    thread* threads;//线程
    int thread_num;//线程数
    int listen_fd;//监听fd
    int epollfd;//主epollfd
    int stop;//全部停止标志
    static threadpool* instance;//单例实例
};
threadpool::threadpool(int listenfd,int thread_number):
                                            listen_fd(listenfd),
                                            thread_num(thread_number),
                                            stop(0)
{
    assert(thread_number>0&&thread_number<=MAX_THREAD_NUMBER);
    threads=new thread[thread_number];
    assert(threads);
}
threadpool* threadpool::instance=NULL;

void threadpool::init()
{
    for(int i=0;i<thread_num;++i)
    {
        int ret=socketpair(PF_UNIX,SOCK_STREAM,0,threads[i].pipe_fd);
        assert(ret==0);
        threads[i].idx=i;
        pthread_create(&threads[i].pid,NULL,thread_func,&threads[i].idx);
        assert(threads[i].pid>=0);
    }
}

void addsig(int sig,void(handler)(int),bool restart=true)
{
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler=handler;
    if(restart)
        sa.sa_flags|=SA_RESTART;
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig,&sa,NULL)!=-1);
}
void threadpool::run()//主线程
{
    epollfd=epoll_create(5);
    assert(epollfd!=-1);
    int sub_counter=0;//轮循
    int new_conn=1;//sockpair发送
    addfd(epollfd,listen_fd);
    epoll_event events[MAX_EVENT_NUMBER];
    int number=0;
    while(!stop)
    {
        number=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if(number<0&&errno!=EINTR)//中断
        {
            printf("epoll failure\n");
            break;
        }
        for(int i=0;i<number;++i)
        {
            int sockfd=events[i].data.fd;
            if(sockfd==listen_fd)
            {
                /*新连接 用Round Robin轮转方式分配给工作线程*/
                sub_counter=(sub_counter+1)%thread_num;
                send(threads[sub_counter].pipe_fd[0],(char*)&new_conn,sizeof(new_conn),0);
                printf("send connect request to child %d\n",sub_counter);
            }
            else
            {
                continue;
            }
        }
    }
    //close(m_listenfd);/*由创建者关闭这个文件描述符*/
    close(epollfd);
}
threadpool* tp=NULL;
void threadpool::setup_sig_pipe()
{
    /*创建epoll事件监听表和信号管道*/
    addsig(SIGPIPE,SIG_IGN);
}

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

/*删除fd注册事件*/
static void removefd(int epollfd,int fd)
{
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
    close(fd);
    delete buffer_map[fd];
    buffer_map[fd]=NULL;
    buffer_map.erase(fd);
}

void* thread_func(void* arg)
{
    int idx=*(int*)arg;
    tp->run_loop(idx);
}
//子线程
void threadpool::run_loop(int idx)
{
    printf("ide=%d\n",idx);
    threads[idx].epollfd=epoll_create(5);
    int epollfd=threads[idx].epollfd;
    assert(epollfd!=-1);
    int pipefd=threads[idx].pipe_fd[1];
    /*子进程需要监听管道文件描述符pipefd，因为父进程通过它来通知子进程accept新连接*/
    addfd(epollfd,pipefd);

    epoll_event events[MAX_EVENT_NUMBER];
    int number=0;
    int ret=-1;
    while(!stop)
    {
        number=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if(number<0&&errno!=EINTR)
        {
            printf("epoll failure\n");
            break;
        }
        for(int i=0;i<number;++i)
        {
            int sockfd=events[i].data.fd;
            //连接到达
            if(sockfd==pipefd&&events[i].events&EPOLLIN)
            {
                printf("conn arrive to %d thread\n",idx);
                int client=0;
                /*从父子进程管道读取数据，并将结果保存在变量client中。如果读取成功，则表示有客户连接到来*/
                ret=recv(sockfd,(char*)&client,sizeof(client),0);
                if((ret<0&&errno==EAGAIN)||ret==0)
                {
                    continue;
                }
                else
                {
                    struct sockaddr_in client_address;
                    socklen_t client_addlength=sizeof(client_address);
                    int connfd=accept(listen_fd,(struct sockaddr*)&client_address,&client_addlength);
                    //map connfd,Buffer,一个连接一个buffer
                    buffer_map[connfd]=new Buffer;
                    if(connfd<0)
                    {
                        printf("errno is %d\n",errno);
                        continue;
                    }
                    addfd(epollfd,connfd);
                }
            }
            /*其他可读数据，必然是客户请求到来，调用逻辑处理对象的process方法处理*/
            else if(events[i].events&EPOLLIN)
            {
                printf("event trigger once\n");
                Buffer* buffer=buffer_map[sockfd];
                while(buffer->len<buffer->length)
                {
                    int ret=recv(sockfd,buffer->buf+buffer->len,buffer->length-buffer->len,0);
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
                        removefd(epollfd,sockfd);
                        break;
                    }
                    else if(ret==0)
                    {
                        printf("connect down\n");
                        removefd(epollfd,sockfd);
                    }
                    else
                    {
                        buffer->len+=ret;
                        if(buffer->len>=4)
                            buffer->length=trans4(buffer->buf);
                        if(buffer->len>=buffer->length)
                        {
                            Result *x=process_request(buffer->buf);
                            send_response(sockfd,x);
                            buffer->len=0;
                            buffer->length=PROTO_SIZE;
                            memset(buffer->buf,'\0',BUFFER_SIZE);
                        }
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }
    close(pipefd);
    close(epollfd);
}


Result* packet_response(char *str,int status,int type)
{
    //返回肯定是PROTO_SIZE+strlen（json）,json中没有\0
    char* result=new char[PROTO_SIZE];
    Result *res=new Result;
    //将结果打包
    printf("\n response:  status=%d,type=%d\n",status,type);
    if(status==FAILURE||status==INSERT_ERROR||status==GET_MYSQL_ERROR||status==MODIFY_ERROR)
    {
        //错误，无数据
        res->json=NULL;
        unsigned int length=PROTO_SIZE;
        memcpy(result,&length,4);
        memcpy(result+4,&status,2);
        memcpy(result+6,&type,2);
        int CRC=0;
        memcpy(result+8,&CRC,4);
        res->protocol=result;
    }
    else
    {
        if(str!=NULL)
            res->json=str;
        else
            res->json=NULL;
        res->protocol=result;
        int CRC=4;//CRC校验，包括json+4个字段,暂无
        unsigned int length=PROTO_SIZE;
        if(str!=NULL)
            length+=strlen(str);//包总长度
        memcpy(result,&length,4);
        memcpy(result+4,&status,2);
        memcpy(result+6,&type,2);
        memcpy(result+8,&CRC,4);
        //增加64位前缀，用协议层封装
    }
    return res;
}

//处理请求
//buffer表示传来的请求，
struct Result* process_request(char* buffer)
{
    int length=trans4((buffer));
    int status=trans2((buffer+4));
    int type=trans2((buffer+6));
    unsigned int CRC=trans4((buffer));
    char* json=buffer+PROTO_SIZE;
    printf("length=%d,status=%d,type=%d,CRC=%d,json=%s\n",length,status,type,CRC,json);
    //int ret=assert_crc(buffer,CRC);crc检验
    struct Result* response=NULL;
    int state=0;
    char* str=NULL;
    int len_t;
    if(status==REQUEST)
        switch(type)
        {
            case login_t:
            {
                printf("json=%s\n",json);
                User* user=(User*)json2struct(json,USER,&len_t);
                User* user_new=NULL;
                printf("account=%s,password=%s",user->account,user->password);
                user_new=query_my_user(user->account,user->password);
                delete user;
                //printf("state=%d\n",state);
                if(user_new!=NULL)
                    state=SUCCESS;
                str=struct2json(user_new,USER,1);
                delete[] user_new;
                response=packet_response(str,state,type);
                break;
            }
            case query_user_t:
            {
                User* user=(User*)json2struct(json,USER,&len_t);
                int id=user->user_id;
                User* result=query_user(id);//查询然后strcut2json 然后协议
                delete user;
                if(result!=NULL)
                    state=SUCCESS;
                str=struct2json(result,USER_REL,len_t);
                delete[] result;    
                response=packet_response(str,state,type);
                break;
            }
            case query_user_rel_t:
            {
                User_Relation* rel=(User_Relation*)json2struct(json,USER_REL,&len_t);
                User_Relation* result=query_user_rel(rel->user_id,&len_t);
                delete rel;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=FAILURE;
                str=struct2json(result,USER_REL,len_t);
                delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case query_collect_t:
            {
                Collect* collect=(Collect*)json2struct(json,COLLECT,&len_t);
                Collect* result=query_collect(collect->user_id,&len_t);
                delete collect;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=FAILURE;
                str=struct2json(result,COLLECT,len_t);
                delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case query_group_t:
            {
                Group* group=(Group*)json2struct(json,GROUP,&len_t);
                Group* result=query_group(group->user_id,&len_t);
                delete group;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=FAILURE;
                str=struct2json(result,GROUP,len_t);
                delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case query_article_title_t:
            {
                Article* article=(Article*)json2struct(json,ARTICLE,&len_t);
                Article* result=query_article_title(article->user_id,&len_t);
                delete article;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=FAILURE;
                str=struct2json(result,ARTICLE,len_t);
                delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case query_article_t:
            {
                Article* article=(Article*)json2struct(json,ARTICLE,&len_t);
                Article* result=query_article(article->art_id);
                delete article;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=FAILURE;
                str=struct2json(result,ARTICLE,1);
                delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case query_comment_t:
            {
                Comment* comment=(Comment*)json2struct(json,COMMENT,&len_t);
                Comment* result=query_comment(comment->art_id,&len_t);
                delete comment;
                if(result!=NULL)
                    state=SUCCESS;
                else
                    state=SUCCESS;
                str=struct2json(result,COMMENT,len_t);
                if(result!=NULL)
                    delete[] result;
                response=packet_response(str,state,type);
                break;
            }
            case insert_user_t:
            {
                printf("json=%s\n",json);
                User* user=(User*)json2struct(json,USER,&len_t);
                state=query_have_user_account(user->account);
                if(state==0)
                    state=insert_user(user);
                else
                    state=FAILURE;
                delete user;
                printf("state=%d\n",state);
                response=packet_response(NULL,state,type);
                break;
            }
            case insert_article_t:
            {
                printf("json=%s\n",json);
                Article* article=(Article*)json2struct(json,ARTICLE,&len_t);
                state=insert_article(article);
                delete article;
                printf("state=%d\n",state);
                response=packet_response(NULL,state,type);
                break;
            }
            case insert_comment_t:
            {
                printf("json=%s\n",json);
                Comment* comment=(Comment*)json2struct(json,COMMENT,&len_t);
                state=insert_comment(comment);
                delete comment;
                printf("state=%d\n",state);
                response=packet_response(NULL,state,type);
                break;
            }
            case modify_user_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER,&len_t);
                state=modify_user(user);//查询然后strcut2json 然后协议
                response=packet_response(NULL,state,type);
                break;
            }
            case modify_article_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER,&len_t);
                state=modify_user(user);//查询然后strcut2json 然后协议
                response=packet_response(NULL,state,type);
                break;               
            }
            case delete_user_t:
            {
                User* user=new User;
                user=(User*)json2struct(json,USER,&len_t);
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
    //发送的时候没有带\0,可能有错
    int index=0;
    int length_t=0;
    int len_t=0;
    if(result->protocol!=NULL)
        while(len_t!=PROTO_SIZE)
        {
            index=write(fd,result->protocol+len_t,PROTO_SIZE-len_t);
            if(index<0)
            {
                printf("send error,ret=%d,errno=%d\n",index,errno);
                return -1;
            }
            else if(index==0)
            {
                printf("connect down\n");
                return -1;
            }
            else
            {
                printf("send %d bytes\n",index);
                len_t+=index;
            }
        }
    if(result->json!=NULL)
    {
        index=0;
        len_t=0;
        length_t=strlen(result->json);
        while(len_t!=length_t)
        {
            index=write(fd,result->json+len_t,length_t-len_t);
            if(index<0)
            {
                printf("send error,ret=%d,errno=%d\n",index,errno);
                return -1;
            }
            else if(index==0)
            {
                printf("connect down\n");
                return -1;
            }
            else
            {
                printf("send %d bytes\n",index);
                len_t+=index;
            }
        }
    }
    return length_t+PROTO_SIZE;
}

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
int main(int argc,char* argv[])
{
    if(argc<=2)
    {
        printf("usage: %s ip_address port_number thread_num\n",basename(argv[0]));
        return 1;
    }
    int thread_num=1;
    if(argc>=4)
    {
        thread_num=atoi(argv[3]);
        printf("thread_num=%d\n",thread_num);
    }
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
    mysql_init();

    tp=threadpool::create(listenfd,thread_num);
    tp->init();
    tp->run();
    close(listenfd);
    return 0;
}