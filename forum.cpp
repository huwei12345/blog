#include "forum.h"
#include "ui_forum.h"

forum::forum(QWidget *parent) :
    QWidget(parent),
    tcpClient(NULL),
    user(NULL),
    mainwindow(NULL),
    search(NULL),
    ui(new Ui::forum)
{
    ui->setupUi(this);
}

forum::~forum()
{
    delete ui;
}

void forum::on_pushButton_6_clicked()
{
    //返回按钮
    father->show();
    this->close();
}

void forum::setfather(QWidget *father)
{
    this->father=father;
}

void forum::update_article_bytype(Article *article, int count)
{
    if(article!=NULL)
    {
        int type=article->type;
        switch(type)
        {
        case 1:
        {
            ui->listWidget_4->clear();
            for(int i=0;i<count;i++)
                ui->listWidget_4->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
            break;
        }
        case 2:
        {
            ui->listWidget_3->clear();
            for(int i=0;i<count;i++)
                ui->listWidget_3->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
            break;
        }
        case 3:
        {
            ui->listWidget->clear();
            for(int i=0;i<count;i++)
                ui->listWidget->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
            break;
        }
        case 4:
        {
            ui->listWidget_2->clear();
            for(int i=0;i<count;i++)
                ui->listWidget_2->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
            break;
        }
        }
    }
}

void forum::update_article_bynow(Article *article, int count)
{
    ui->listWidget_9->clear();
    if(article!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget_9->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
    }
}

void forum::update_article_byweek(Article *article, int count)
{
    ui->listWidget_7->clear();
    if(article!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget_7->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
    }
}

void forum::update_article_bymonth(Article *article, int count)
{
    ui->listWidget_8->clear();
    if(article!=NULL)
    {
        for(int i=0;i<count;i++)
            ui->listWidget_8->addItem(QString("user:")+QString::number(article[i].user_id)+" art_id:"+QString::number(article[i].art_id)+" "+QString(article[i].title));
    }
}

void forum::on_pushButton_5_clicked()
{
    //搜索
    if(search==NULL)
        search=new Search;
    search->setfather(this);
    search->mainwindow=this->mainwindow;
    search->tcpClient=this->tcpClient;
    int index=ui->comboBox->currentIndex();
    switch(index)
    {
    case 0:
    {
        int id=ui->lineEdit->text().toInt();
        if(id!=0)
        {
            //根据id查找各种用户
            User* user=new User;
            user->user_id=id;
            //buffer 状态 请求类型 json的对象引用，数目，对象类型
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_user_id_t;//登录，返回User
            char* str=NULL;
            //如果有对象数据要传输
            str=struct2json(user,USER,1);
            int length=package_request(buf,str,type);

            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            delete user;
        }
        else
        {
            QMessageBox::information(this,"查找失败","输入id不合法");
            return;
        }
        //用户id
        break;
    }
    case 1:
    {
        //用户名
        QString name=ui->lineEdit->text();
        if(name!=NULL&&name.length()<40)
        {
            //根据name查找各种用户
            User* user=new User;
            user->name=name.toUtf8().data();
            //buffer 状态 请求类型 json的对象引用，数目，对象类型
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_user_name_t;//登录，返回User
            char* str=NULL;
            //如果有对象数据要传输
            str=struct2json(user,USER,1);
            int length=package_request(buf,str,type);
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            delete user;
        }
        else
        {
            QMessageBox::information(this,"查找失败","输入name不合法或长度超过40");
            return;
        }
        //用户id
        //根据名字查找各种用户
        break;
    }
    case 2:
    {
        //根据id查找各种文章
        //文章id
        int id=ui->lineEdit->text().toInt();
        if(id!=0)
        {
            Article* article=new Article;
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_article_id_t;//登录，返回User
            char* str=NULL;
            article->art_id=id;
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            int length=package_request(buf,str,type);
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            delete article;
        }
        else
        {
            QMessageBox::information(this,"查找失败","输入id不合法");
            return;
        }
        break;
    }
    case 3:
        //文章名
        //根据文章名查找各种文章
        QString name=ui->lineEdit->text();
        if(name!=NULL&&name.length()<40)
        {
            Article* article=new Article;
            char *buf=new char[500];
            memset(buf,'\0',500);
            short type=query_article_name_t;//登录，返回User
            char* str=NULL;
            article->title=name.toUtf8().data();
            //如果有对象数据要传输
            str=struct2json(article,ARTICLE,1);
            int length=package_request(buf,str,type);
            //发送
            tcpClient->write(buf,length); //qt5去除了.toAscii()
            tcpClient->waitForBytesWritten();
            tcpClient->flush();
            delete[] buf;
            delete article;
        }
        else
        {
            QMessageBox::information(this,"查找失败","输入name不合法或长度超过40");
            return;
        }
        break;
    }
    search->show();
    this->hide();
}
int forum::get_user_id(QListWidgetItem *item)
{
    char* str=item->text().toUtf8().data();
    str+=5;//user:
    int i=0;
    int num=0;
    while(str[i]>='0'&&str[i]<='9')
    {
        num+=num*10+str[i]-'0';
        i++;
    }
    return num;
}

void forum::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
        int num=get_user_id(item);
        qDebug()<<"double click user_id:"<<num;
        mainwindow->user=new User;
        mainwindow->user->user_id=num;
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
        this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_4_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_8_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_7_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_9_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_5_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_6_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}


void forum::on_listWidget_3_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}

void forum::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
    int num=get_user_id(item);
    qDebug()<<"double click user_id:"<<num;
    mainwindow->user=new User;
    mainwindow->user->user_id=num;
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
    this->hide(); //主界面隐藏--后期直接可以显示
}
