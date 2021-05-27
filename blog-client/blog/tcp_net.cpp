#include "tcp_net.h"
#include "ui_tcp_net.h"
//tcp封装类设置ip:port
Tcp_net::Tcp_net(QWidget *parent) :
    QWidget(parent),
    tcpClient(NULL),
    col(NULL),
    focus(NULL),
    forum(NULL),
    mainwindow(NULL),
    form(NULL),
    new_win(NULL),
    length(0),
    len(0),
    ui(new Ui::Tcp_net)
{
    ui->setupUi(this);
    tcpClient=new QTcpSocket(this);
    tcpClient->abort();
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ReadData()));
    //connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));
    tcpClient->connectToHost("192.168.1.102", 10000);
    if (tcpClient->waitForConnected(1000))  // 连接成功则进入if{}
        qDebug()<<"建立连接"<<endl;
    m_buffer=new char[BUFFER_SIZE];
}
int temp=0;
int start_package=0;
void Tcp_net::ReadData()
{
    int ret=0;
    ret= tcpClient->read(m_buffer+len,BUFFER_SIZE);
    if(ret==0)
    {
        return;
        tcpClient->close();
    }
    else
    {
        //数据到达
        if(ret+len<=BUFFER_SIZE/2)//超过一半就往回重置m_buffer
            len=len+ret;//后续许往回调
        else
        {
            //buffer不够用了,重置
            memcpy(m_buffer,m_buffer+start_package,len+ret-start_package);
            len=ret+ret-start_package;
            start_package=0;
        }

        while(1)
        {
            if(temp==0&&len-start_package>=4)//如果没正在处理包
            {
                length=trans4((m_buffer+start_package));
                temp=1;
            }
            else if(temp==0)
            {
                break;//连length都不够
            }
            else if(len-start_package<length)//temp==1,正在处理包,但还没够,需继续读
            {
                break;
            }
            else//够了一个，但也许够两个。或者一个半
            {
                process_packet(m_buffer+start_package,length);
                start_package=start_package+length;
                length=0;
                temp=0;
            }
        }
    }
}

void Tcp_net::process_packet(char* buffer,int length)
{
    qDebug()<<length;
    buffer=buffer+4;//兼容代码，之前的代码是缺少length4字节的的
    qDebug()<<"处理一个响应";
    int len=length;
    int status=trans2((buffer));
    int type=trans2((buffer+2));
    int CRC=trans4((buffer+4));//暂无
    qDebug()<<"Response:        "<<"status:"<<status<<" type:"<<type<<" length:"<<len;
    if(len>12)
        qDebug()<<"json: "<<buffer+8;
    else
        qDebug()<<"json:"<<"NULL";
    CRC++;
    if(status==SUCCESS&&len>=12)
    {
        int size=0;
        switch(type)
        {
        case login_t:
        {
            User* user=NULL;
            user=(User*)json2struct(buffer+8,USER,&size);
            if(user==NULL)
                QMessageBox::information(this,"无账号信息","无账号信息");
            else
            {
                //new_win->update(user);
                if(new_win==NULL)
                {
                    new_win=new New;
                    new_win->setfather(this->father);
                    new_win->tcpClient=this->tcpClient;//key!!此处将tcpClient传给new Window
                    //提前生成mainwindow，以备关注，收藏跳转
                    if(mainwindow==NULL)
                    {
                        mainwindow=new MainWindow;
                        mainwindow->tcpClient=tcpClient;
                        new_win->mainwindow=mainwindow;
                        mainwindow->setfather(new_win);
                    }
                }
                new_win->set_User(user);
                mainwindow->user=new_win->user;
                mainwindow->my_user=new_win->user;
                new_win->show();
                new_win->father->hide();
            }
            break;
        }
        case query_user_t:
        {
            User* user=NULL;
            user=(User*)json2struct(buffer+8,USER,&size);
            if(user!=NULL)
            {
                if(new_win->user->user_id==user->user_id)
                {
                    new_win->set_User(user);//重新获取了本人信息
                    mainwindow->user=user;
                    mainwindow->my_user=user;
                }
                form=new_win->form;
                form->user=user;
                form->update();
            }
            break;
        }
        case query_user_rel_t:
        {
            new_win->focus->mainwindow=this->mainwindow;
            new_win->focus->tcpClient=tcpClient;
            User_Relation* rel=(User_Relation*)json2struct(buffer+8,USER_REL,&size);
            if(rel!=NULL)
            {
                focus=this->new_win->focus;
                focus->update(rel,size);
            }
            break;
        }
        case query_collect_t:
        {
            new_win->col->mainwindow=this->mainwindow;
            new_win->col->tcpClient=tcpClient;
            Collect* collect=(Collect*)json2struct(buffer+8,COLLECT,&size);
            if(collect!=NULL)
            {
                col=this->new_win->col;
                col->update(collect,size);
            }
            break;
        }
        case query_group_t:
        {
            //有时候查不出来的原因是第二个包先到了
            Group* group=NULL;
            group=(Group*)json2struct(buffer+8,GROUP,&size);
            if(group!=NULL)
            {
                mainwindow->update_group(group,size);
                //对文章标题列表查询
            }
            else
            {
                mainwindow->update_group(NULL,0);
            }
            //查文章标题
            char *buf=new char[500];
            char* str=NULL;
            int length=0;
            memset(buf,'\0',500);
            type=query_article_title_t;//登录，返回User
            str=NULL;
            Article* article=new Article;
            article->user_id=mainwindow->user->user_id;
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            delete article;
            length=package_request(buf,str,type);

            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->flush();
            mainwindow->init_other_blog();
            delete[] buf;
            break;
        }
        case query_article_title_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_article_list(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_article_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_article(article);
            }
            break;
        }
        case query_comment_t:
        {
            Comment* comment=NULL;
            if(len>12)
                comment=(Comment*)json2struct(buffer+8,COMMENT,&size);
            else
                size=0;
                mainwindow=this->new_win->mainwindow;
                mainwindow->update_comment(comment,size);
                tcpClient->flush();
            break;
        }
        case insert_article_t:
        {
            QMessageBox::information(this,"插入文章成功","插入文章成功");
            if(len>12)
            {
                Article* article=NULL;
                article=(Article*)json2struct(buffer+8,ARTICLE,&size);
                if(article!=NULL)
                {
                    mainwindow->insert_article(article,1);
                    tcpClient->flush();
                }
            }
            break;
        }
        case modify_article_t:
        {
            QMessageBox::information(this,"修改文章成功","修改文章成功");
            break;
        }
        case delete_article_t:
        {
            //更新左侧界面，右侧评论和文章等
            break;
        }
        case modify_user_t:
        {
            //修改user
            QMessageBox::information(this,"修改个人信息成功","修改个人信息成功");
            break;
        }
        case query_user_rel_simple_t:
        {
            //查询是否存在关注关系 state=SUCCESS
            mainwindow->set_rel_on();
            break;
        }
        case query_collect_simple_t:
        {
            //查询是否存在关注关系 state=SUCCESS
            mainwindow->set_col_on();
            break;
        }
        case insert_user_rel_t:
        {
            //这四个不用处理
            break;
        }
        case delete_user_rel_t:
        {
            break;
        }
        case insert_collect_t:
        {
            break;
        }
        case delete_collect_t:
        {
            break;
        }
        case add_art_upvote_t:
        {
            //无操作
            break;
        }
        case query_article_bytype_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bytype(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_bynow_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bynow(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_byweek_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_byweek(article,size);
                tcpClient->flush();
            }
            break;
        }
        case query_art_bymonth_t:
        {
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            if(article!=NULL)
            {
                forum=this->new_win->forum;
                forum->update_article_bymonth(article,size);
                tcpClient->flush();
            }
            break;
        }
        case insert_user_t:
        {
            QMessageBox::information(this,"注册成功","注册成功");
            break;
        }
        case insert_group_t:
        {
            Group* group=(Group*)json2struct(buffer+8,GROUP,&size);
            if(group!=NULL)
            {
                mainwindow->update_new_group(group);
            }
            break;
        }
        case delete_group_t:
        {
            mainwindow->delete_group();
            break;
        }
        case query_user_id_t:
        {
            //这四个是搜索功能
            User* user=NULL;
            user=(User*)json2struct(buffer+8,USER,&size);
            this->search=new_win->search;
            search->show_user_id(user,size);
            break;
        }
        case query_user_name_t:
        {
            User* user=NULL;
            user=(User*)json2struct(buffer+8,USER,&size);
            this->search=new_win->search;
            search->show_user_id(user,size);
            break;
        }
        case query_article_id_t:
        {
            this->search=new_win->search;
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            search->show_art_id(article,size);
            break;
        }
        case query_article_name_t:
        {
            this->search=new_win->search;
            Article* article=NULL;
            article=(Article*)json2struct(buffer+8,ARTICLE,&size);
            search->show_art_title(article,size);
            break;
        }
        case delete_comment_t:
        {
            break;
        }
        case modify_group_t:
        {
            mainwindow->modify_father_group();
            break;
        }
        case modify_article_group_t:
        {
            mainwindow->modify_article_group();
            break;
        }
        }
    }
    else if(status==FAILURE)
    {
        if(type==login_t)
            QMessageBox::information(this,"登录错误","账号或者密码错误");
        else if(type==insert_user_t)
            QMessageBox::information(this,"注册失败","该账号已存在");
        else if(type==query_group_t)
        {
            mainwindow->update_group(NULL,0);

            char *buf=new char[500];
            char* str=NULL;
            int length=0;
            memset(buf,'\0',500);
            type=query_article_title_t;//登录，返回User
            str=NULL;
            Article* article=new Article;
            article->user_id=mainwindow->user->user_id;
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            delete article;
            length=package_request(buf,str,type);
            qDebug()<<length<<"  "<<buf+12;
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            delete[] buf;
            tcpClient->waitForBytesWritten();
            tcpClient->flush();

            //是否其他人的博客
            mainwindow->init_other_blog();
        }
        else if(type==query_user_rel_simple_t)
        {
            mainwindow->set_rel_down();
        }
        else if(type==query_collect_simple_t)
        {
            mainwindow->set_col_down();
        }
        else if(type==add_art_upvote_t)
        {
        }
        else if(type==insert_user_rel_t)
        {
        }
        else if(type==delete_user_rel_t)
        {
        }
        else if(type==query_collect_simple_t)
        {
        }
        else if(type==insert_collect_t)
        {
        }
        else if(type==delete_collect_t)
        {
        }
        else if(type==query_article_bytype_t)
        {

        }
        else if(type==query_art_bynow_t)
        {
        }
        else if(type==query_art_byweek_t)
        {
        }
        else if(type==query_art_bymonth_t)
        {
        }
        else if(type==modify_article_t)
        {
            QMessageBox::information(this,"修改文章失败","修改文章失败");
        }
        else if(type==insert_article_t)
        {
            QMessageBox::information(this,"添加文章失败","添加文章失败");
        }
        else if(type==delete_group_t)
        {
            QMessageBox::information(this,"删除分组失败","删除分组失败，请检查分组内是否有文章或分组");
        }
        else if(type==query_user_id_t)
        {
            this->search=new_win->search;
            search->show_user_id(NULL,0);
        }
        else if(type==query_user_name_t)
        {
            this->search=new_win->search;
            search->show_user_name(NULL,0);
        }
        else if(type==query_article_id_t)
        {
            this->search=new_win->search;
            search->show_art_id(NULL,0);
        }
        else if(type==query_article_name_t)
        {
            this->search=new_win->search;
            search->show_art_title(NULL,0);
        }
        else if(type==modify_group_t)
        {
            QMessageBox::information(this,"修改分组失败","修改分组父分组失败");
        }
        else if(type==modify_article_group_t)
        {
            QMessageBox::information(this,"修改分组失败","修改文章父分组失败");
        }
    }
    else
    {}
}

Tcp_net::~Tcp_net()
{
    tcpClient->disconnectFromHost();
    if (tcpClient->state() == QAbstractSocket::UnconnectedState \
                    || tcpClient->waitForDisconnected(1000))
    {
        qDebug()<<"关闭连接";
    }
    tcpClient->close();
    delete tcpClient;
    delete ui;
}

void Tcp_net::setfather(QWidget *father)
{
    this->father=father;
}
