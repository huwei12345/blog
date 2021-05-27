#include "search.h"
#include "ui_search.h"

Search::Search(QWidget *parent) :
    QWidget(parent),
    now_type(0),
    tcpClient(NULL),
    ui(new Ui::Search),
    father(NULL)
{
    ui->setupUi(this);
}

Search::~Search()
{
    delete ui;
}

void Search::setfather(QWidget *father)
{
    this->father=father;
}

void Search::show_art_id(Article *article, int count)
{
    now_type=1;
    ui->listWidget->clear();
    if(article!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget->addItem(QString("user:")+QString::number(article[i].user_id)+QString("    art_id:")+QString::number(article[i].art_id)+QString("    title:")+QString(article[i].title));
        delete[] article;
    }
}

void Search::show_art_title(Article *article, int count)
{
    now_type=1;
    ui->listWidget->clear();
    if(article!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget->addItem(QString("user:")+QString::number(article[i].user_id)+QString("    art_id:")+QString::number(article[i].art_id)+QString("    title:")+QString(article[i].title));
        delete[] article;
    }
}

void Search::show_user_id(User *user, int count)
{
    now_type=0;
    ui->listWidget->clear();
    if(user!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget->addItem(QString("user:")+QString::number(user[i].user_id)+QString("    user_name:")+QString(user[i].name));
        delete[] user;
    }
}

void Search::show_user_name(User *user, int count)
{
    now_type=0;
    ui->listWidget->clear();
    if(user!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget->addItem(QString("user:")+QString::number(user[i].user_id)+QString("    user_name:")+QString(user[i].name));
        delete[] user;
    }
}

void Search::on_pushButton_clicked()
{
    father->show();
    this->close();
}

void Search::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(now_type==1)
    {
        //文章
        QString str=item->text();
        char* s=str.toUtf8().data()+5;
        int num=0;
        int i=0;
        while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
        {
            num=num*10+(s[i]-'0');
            i++;
        }
        if(num!=0)
        {
            mainwindow->user=new User;
            mainwindow->user->user_id=num;
            //mainwindow需要先初始化
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_group_t;//登录，返回User
            char* str=NULL;
            Group* group=new Group;
            group->user_id=num;
            //如果有对象数据要传输
            str=struct2json(group,GROUP,1);
            delete group;
            int length=package_request(buf,str,type);
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            mainwindow->show();
            this->hide();
        }
    }
    else if(now_type==0)
    {
        //用户
        QString str=item->text();
        char* s=str.toUtf8().data()+5;
        int num=0;
        int i=0;
        while(s[i]!='\0'&&s[i]>='0'&&s[i]<='9')
        {
            num=num*10+(s[i]-'0');
            i++;
        }
        if(num!=0)
        {
            mainwindow->user=new User;
            mainwindow->user->user_id=num;
            //mainwindow需要先初始化
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_group_t;//登录，返回User
            char* str=NULL;
            Group* group=new Group;
            group->user_id=num;
            //如果有对象数据要传输
            str=struct2json(group,GROUP,1);
            delete group;
            int length=package_request(buf,str,type);
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            mainwindow->show();
            this->hide();
        }
    }
}
