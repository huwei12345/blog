#include "new.h"
#include "ui_new.h"


/*
可以用stackwidget代替各种切换界面，更加科学，用一些button返回和跳转，返回不必每次返回上一层，这样内存会保存太多页面
目前只需要返回主页面，每个页面，跳转过后就删除。

*/
New::New(QWidget *parent) :
    QWidget(parent),
    focus(NULL),
    col(NULL),
    form(NULL),
    mainwindow(NULL),
    user(NULL),
    ui(new Ui::New)
{
    ui->setupUi(this);
    QPixmap *pixmap = new QPixmap(":/work/194977972.jpg");
    pixmap->scaled(ui->label_2->size(),Qt::IgnoreAspectRatio);//把图片
    ui->label_2->setScaledContents(true);
    ui->label_2->setPixmap(*pixmap);
}

New::~New()
{
    delete ui;
    qDebug()<<"New 窗口析构";
}

void New::set_User(User *user)
{
    if(this->user==user)
        return;
    if(this->user!=NULL)
        delete this->user;
    this->user=user;
}

void New::setfather(QWidget *father)
{
    this->father=father;
}

void New::on_pushButton_5_clicked()
{
    //注销
    father->show();
    this->close();
    //delete this; 对的 ???
}

void New::on_pushButton_6_clicked()
{
    //退出
    this->close();
    //delete this;
    father->close();
    delete this;
}

void New::on_pushButton_clicked()
{
    //博客
    //对文章分组
    //buffer 状态 请求类型 json的对象引用，数目，对象类型
    char *buf=new char[500];
    memset(buf,'\0',500);
    mainwindow->user=user;
    short type=query_group_t;//登录，返回User
    char* str=NULL;
    Group* group=new Group;
    group->user_id=user->user_id;
    //如果有对象数据要传输
    str=struct2json(group,GROUP,1);
    delete group;
    int length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();

    //不能在这里发送第二个请求，因为接收可能顺序有问题，要在接收到第一个响应后在发送
    delete[] buf;
    mainwindow->show();
    this->hide(); //主界面隐藏--后期直接可以显示
}

void New::on_pushButton_2_clicked()
{
    //关注
    if(focus==NULL)
        focus=new class focus;
    focus->setfather(this);
    focus->user=user;
    focus->tcpClient=tcpClient;
    //buffer 状态 请求类型 json的对象引用，数目，对象类型
    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=query_user_rel_t;//登录，返回User
    char* str=NULL;
    User_Relation* rel=new User_Relation;
    rel->user_id=user->user_id;
    //如果有对象数据要传输
    str=struct2json(rel,USER_REL,1);
    delete rel;
    int length=package_request(buf,str,type);

    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();

    delete[] buf;

    focus->show();
    this->hide(); //主界面隐藏--后期直接可以显示
}

void New::on_pushButton_3_clicked()
{
    //收藏
    if(col==NULL)
        col=new Col;
    col->setfather(this);
    col->user=user;
    col->tcpClient=tcpClient;
    //buffer 状态 请求类型 json的对象引用，数目，对象类型
    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=query_collect_t;//登录，返回User
    char* str=NULL;
    Collect* collect=new Collect;
    collect->user_id=user->user_id;
    //如果有对象数据要传输
    str=struct2json(collect,COLLECT,1);
    delete collect;
    int length=package_request(buf,str,type);

    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->flush();

    delete[] buf;

    col->show();
    this->hide(); //主界面隐藏--后期直接可以显示
}

void New::on_pushButton_4_clicked()
{
    //论坛
    if(forum==NULL)
        forum=new class forum;
    search=new Search;
    forum->search=search;
    forum->setfather(this);
    forum->user=user;
    forum->tcpClient=tcpClient;
    forum->mainwindow=this->mainwindow;
    char *buf=new char[500];
    short type=query_article_bytype_t;//登录，返回User
    char* str=NULL;
    Article* article=new Article;
    int a[]={1,2,3,4};
    for(unsigned int i=0;i<sizeof(a)/sizeof(a[0]);i++)
    {
        memset(buf,'\0',500);
        article->type=a[i];
        //如果有对象数据要传输
        str=struct2json(article,ARTICLE,1);
        int length=package_request(buf,str,type);
        //发送
        tcpClient->write(buf,length); //qt5去除了.toAscii()
        tcpClient->waitForBytesWritten();
        tcpClient->flush();
    }

    str=NULL;
    memset(buf,'\0',500);
    type=query_art_bynow_t;
    int length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();

    str=NULL;
    memset(buf,'\0',500);
    type=query_art_bymonth_t;
    length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();

    str=NULL;
    memset(buf,'\0',500);
    type=query_art_byweek_t;
    length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();

    delete article;
    delete[] buf;

    //时间类3个

    forum->show();
    this->hide(); //主界面隐藏--后期直接可以显示
}

void New::on_pushButton_7_clicked()
{
    //个人信息
    if(form==NULL)
        form=new Form;
    form->setfather(this);
    form->tcpClient=tcpClient;

    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=query_user_t;//登录，返回User
    char* str=NULL;
    User* user=new User;
    user->user_id=this->user->user_id;
    //如果有对象数据要传输
    str=struct2json(user,USER,1);
    delete user;
    int length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;

    form->show();
    this->hide();
}
