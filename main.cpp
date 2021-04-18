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
//毕业设计服务器端 2020/2/26

struct httpRequest{
    int method;//请求方法
    char url[50];//url
    char** headname;//头部字段名key
    char** value;//值
    char* content;//内容
    bool isright;
};

struct Http_Response{
    char* version;//HTTP/1.1
    int statevalue;//状态码
    char* state;//状态描述
    char** headname;//头部key
    char** value;//头部value
    char* content;//传送内容
    bool isright;//是否成功传送
};
char* content=NULL;
//状态机，主状态机：正在读请求行，正在读头部字段
enum CHECK_STATE{CHECK_STATE_REQUESTLINE=0,CHECK_STATE_HEADER};

//从状态机有三种状态，行的读取状态：读取到一个完整行、行出错、行数据尚且不完整
enum LINE_STATUS{LINE_OK=0,LINE_BAD,LINE_OPEN};

/*服务器处理HTTP请求的结果：NO_REQUEST 请求不完整 需继续读取客户数据 ，GET_REQUEST 表示获得一个完整的客户请求
    BAD_REQUEST 请求语法错误  FORBIDDEN_REQUEST表示客户对资源没有足够访问权限，INTERNAL_ERROR 表示服务器内部
    错误，CLOSED_CONNECTION表示客户端已经关闭连接了*/
enum HTTP_CODE {NO_REQUEST,GET_REQUEST,BAD_REQUEST,FORBIDDEN_REQUEST,INTERNAL_REQUEST,CLOSED_CONNECTION};
/* 为简化，我们没给客户端发送完整报文，根据服务器处理结果发送如下信息*/


int send_http_response(int fd,HTTP_CODE state);//发送http
static const char* szret[]={"I get a correct result\n","Something wrong\n"};
/*从状态机，用于解析出一行内容*/
//解耦成处理输入，返回一个对应输入的指针，和一个处理输出，接收输入的结构体指针，处理完输出，返回另一个结构体
/*从状态机，用于解析出一行内容*/
LINE_STATUS parse_line(char* buffer,int& checked_index,int& read_index)
{
    //check_index：当前分析字符
    //read_index：buffer中客户数据的尾部的下一字符
    //check_index之前都已分析，当前分析check_index~read_index-1字符
    char temp;
    for(;checked_index<read_index;++checked_index)
    {
        //获取当前分析字符
        temp=buffer[checked_index];
        /*如果当前字符是/r，则可能读到完整行*/
        if(temp=='\r')
        {
            /*如果\r 字符是目前buffer中最后一个已经被读取的客户数据，那么没有读到完整的行，返回LINE_OPEN*/
            if(checked_index+1==read_index)
                return LINE_OPEN;
            /*下一个字符是\n,则读取到一个完整行*/
            else if(buffer[checked_index+1]=='\n')
            {
                buffer[checked_index++]='\0';
                buffer[checked_index++]='\0';
                return LINE_OK;
            }
            /*语法有问题*/
            return LINE_BAD;
        }
        /*如果当前字符是\n,则也说明读到一个完整行*/
        else if(temp=='\n')
        {
            if(checked_index>1&&buffer[checked_index-1]=='\r')
            {
                buffer[checked_index-1]='\0';
                buffer[checked_index++]='\0';
                return LINE_OK;
            }
        }
    }
    /*如果未读到"\r",则返回LINE_OPEN,表示还需继续读取*/
    return LINE_OPEN;
}

/*分析请求行*/
HTTP_CODE parse_requestline(char* temp,CHECK_STATE& checkstate)
{
    char* url=strpbrk(temp," \t");
    /*如果请求行中没有“ ”或者“\t”，必有问题*/
    if(!url)
    {
        return BAD_REQUEST;
    }
    char* method=temp;
    if(strcasecmp(method,"GET")==0)
    {
        printf("the request method is GET\n");//仅支持GET方法
    }
    else
    {
        return BAD_REQUEST;
    }
    url+=strspn(url," \t");
    char* version=strpbrk(url," \t");//是在源字符串（s1）中找出最先含有搜索字符串（s2）中任一字符的位置并返回，若找不到则返回空指针。
    if(!version)
    {
        return BAD_REQUEST;
    }
    *version++='\0';
    version+=strspn(version," \t");//检索str1中第一个不在str2中的字符
    /*仅支持HTTP/1.1*/
    if(strcasecmp(version,"HTTP/1.1")!=0)
    {
        return BAD_REQUEST;
    }
    /*检查URL是否合法*/
    if(strncasecmp(url,"http://",7)==0)//比较字符串前n个字符，忽略大小写
    {
        url+=7;
        url=strchr(url,'/');//在一个串中查找给定字符的第一个匹配之处。
    }
    if(!url||url[0]!='/')
        return BAD_REQUEST;
    printf("the request URL is %s\n",url);
    checkstate=CHECK_STATE_HEADER;
    return NO_REQUEST;
}

/*分析头部字段*/
HTTP_CODE parse_headers(char* temp)
{
    /*遇到一个空行，说明得到一个正确的HTTP请求*/
    if(temp[0]=='\0')
    {
        return GET_REQUEST;
    }
    else if(strncasecmp(temp,"Host:",5)==0)//处理HOST头部字段
    {
        temp+=5;
        temp+=strspn(temp," \t");
        printf("the request host is:%s\n",temp);
    }
    else//其他都不处理
    {
        printf("I can not handle this header\n");
    }
    return NO_REQUEST;
}

/*分析HTTP请求的入口函数*/
HTTP_CODE parse_content(char* buffer,int& checked_index,CHECK_STATE& checkstate,int& read_index,int& start_line)
{
    LINE_STATUS linestatus=LINE_OK;//记录当前行的读取状态
    HTTP_CODE retcode =NO_REQUEST;//记录HTTP请求的处理结果
    /*主状态机，用于从buffer中取出所有完整的行*/
    while((linestatus=parse_line(buffer,checked_index,read_index))==LINE_OK)
    {
        char* temp=buffer+start_line;//start_line是行在buffer中的起始位置
        start_line=checked_index;//记录下一行的起始位置
        /*主状态机，用于从buffer中取出所有完整的行*/
        switch(checkstate)
        {
            case CHECK_STATE_REQUESTLINE:
            {
                retcode=parse_requestline(temp,checkstate);
                if(retcode==BAD_REQUEST)
                    return BAD_REQUEST;
                break;
            }
            case CHECK_STATE_HEADER:
            {
                retcode=parse_headers(temp);
                if(retcode==BAD_REQUEST)
                    return BAD_REQUEST;
                else if(retcode==GET_REQUEST)
                    return GET_REQUEST;
                break;
            }
            default:
            {
                return INTERNAL_REQUEST;
            }
        }
    }
    if(linestatus==LINE_OPEN)
    {
        return NO_REQUEST;
    }
    else
    {
        return BAD_REQUEST;
    }
}

int process_http(int epollfd,int fd)//读取http请求
{
    int idx=0;
    int ret=0;
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',BUFFER_SIZE);
    int data_read=0;
    
    int read_index=0;//当前已读取多少字节客户数据
    int checked_index=0;//当前已分析完多少字节客户数据
    
    int start_line=0;//行在buffer中的起始位置
    CHECK_STATE checkstate=CHECK_STATE_REQUESTLINE;
    while(1)//循环读取客户数据并分析之
    {
        data_read=recv(fd,buffer+read_index,BUFFER_SIZE-read_index,0);
        if(data_read==-1)
        {
            printf("reading failed\n");
            break;
        }
        else if(data_read==0)
        {
            printf("rempte client has closed the connection\n");
            break;
        }
        read_index+=data_read;
        /*分析目前已获取的所有客户数据*/
        HTTP_CODE result=parse_content(buffer,checked_index,checkstate,read_index,start_line);
        if(result==NO_REQUEST)
            continue;
        else if(result==GET_REQUEST)
        {
            send_http_response(fd,GET_REQUEST);//根据亲求发送相应
            break;
        }
        else
        {
            send_http_response(fd,BAD_REQUEST);
            break;
        }
    }

/*
    int idx=0;
    int ret=0;
    char buffer[BUFFER_SIZE];
    while(true)
    {
        ret=read(fd,buffer,BUFFER_SIZE);
        if(ret<0)
        {
            if(errno!=EAGAIN)
            {
                epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
                close(fd);
                break;
            }
        }
        else if(ret==0)//对端关闭
        {
            epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);
            close(fd);
            break;
        }
        else
        {
            idx+=ret;//读取get请求
            for(int i=0;i<idx;++i)
            {

            }
        }
    }
*/
}
void package_http(struct Http_Response *res,int length)
{
    //封装http相应
    int index=0;
    char response[]="HTTP/1.1 200 OK\r\n";
    int res_len=strlen(response);
    snprintf(content,res_len,"HTTP/1.1 200 OK\r\n");
    index+=res_len;
    char Content_Type[]="Content-Type: text/html;charset=utf-8\r\n";
    int Cont_len=strlen(Content_Type);
    snprintf(content+index,Cont_len,"%s",Content_Type);
    index+=Cont_len;
    char Content_length[100]="Content-Length: ";
    snprintf(content+index,200,"%s%d\r\n\r\n",Content_length,length);
}
void send_response_200(int fd)
{
    int ret=0;
    char buffer[CONTENT_SIZE];
    int indexfd=open("./index.html",O_RDWR);
    int content_length=0;
    while((ret=read(indexfd,buffer+content_length,CONTENT_SIZE))!=0)
    {
        if(ret>=0)
            content_length+=ret;
    }
    Http_Response *response=NULL;
    package_http(response,content_length);
    
    ret=write(fd,content,strlen(content));
    
    ret=write(fd,buffer,content_length);    
    
    close(indexfd);
}
void send_response_404(int fd)
{
    char message_404[]="HTTP/1.1 404 Not Found\r\n";
    int res_len=strlen(message_404);
    int index=res_len;
    char Content_Type[]="Content-Type: text/html;charset=utf-8\r\n";
    int Cont_len=strlen(Content_Type);
    snprintf(content+index,Cont_len,"%s",Content_Type);
    index+=Cont_len;
    char Content_length[100]="Content-Length: ";
    snprintf(content+index,200,"%s%d\r\n\r\n",Content_length,0);
    write(fd,content,strlen(content));
}
int send_http_response(int fd,HTTP_CODE state)//发送http
{
    if(state==BAD_REQUEST)
        send_response_404(fd);
    else if(state==GET_REQUEST)
    {
        send_response_200(fd);
    }
    else if(state==CLOSED_CONNECTION)
    {
        //Close();
    }
}

int main(int argc,char* argv[])
{
    int servsock;
    int epollfd;
    int port;
    if(argc<=2)
    {
        perror("usage:./filename ip port\n");
        return 1;
    }
    content=(char*)malloc(CONTENT_SIZE);
    char* ip=argv[1];
    port=atoi(argv[2]);
    struct sockaddr_in sockaddr;
    memset(&sockaddr,0,sizeof(sockaddr));
    sockaddr.sin_family=AF_INET;
    sockaddr.sin_port=htons(port);
    int ret=inet_pton(AF_INET,ip,&sockaddr.sin_addr);
    assert(ret>=0);
    
    servsock=socket(PF_INET,SOCK_STREAM,0);
    printf("%d\n",servsock);
    ret=bind(servsock,(struct sockaddr*)&sockaddr,sizeof(sockaddr));
    assert(ret!=-1);
    
    ret=listen(servsock,5);
    printf("111111111\n");
    
    assert(ret!=-1);
    
    epollfd=epoll_create(10);
    struct epoll_event events[EPOLLNUM];
    struct epoll_event event;
    event.data.fd=servsock;
    event.events=EPOLLIN;
    
    sockaddr_in clientaddr;
    socklen_t clientlen=sizeof(clientaddr);
    int clientfd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,servsock,&event);
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
            if(events[i].events|EPOLLIN&&events[i].data.fd==servsock)
            {
                //连接到来
                clientfd=accept(servsock,(struct sockaddr*)&clientaddr,&clientlen);
                event.data.fd=clientfd;
                event.events=EPOLLIN;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&event);//添加到epoll
                //http 连接
                printf("http connect success\n");
            }
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
        }
    }
    return 0;
}
