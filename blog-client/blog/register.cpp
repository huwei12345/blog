#include "register.h"
#include "ui_register.h"
#include "splash.h"
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register),
    father(NULL)
{
    ui->setupUi(this);
}

Register::~Register()
{
    delete ui;
}

void Register::setfather(QWidget *father)
{
    this->father=father;
}

void Register::on_pushButton_clicked()
{
    //返回
    father->show();
    this->close();
    delete this;
}

void Register::on_pushButton_2_clicked()
{
    //注册完并返回
    //。。。。。远程注册
    User* user_t=new User;
    QString str=ui->lineEdit->text();
    if(str==NULL)
    {
        QMessageBox::information(this,"注册失败","昵称缺少");
        delete user_t;
        return;
    }
    else
        user_t->name=str.toUtf8().data();
    str=ui->lineEdit_2->text();
    if(str==NULL)
    {
        qDebug()<<"性别缺少";
    }
    else
        user_t->sex=str.toUtf8().data();
    str=ui->lineEdit_3->text();
    if(str==NULL)
        qDebug()<<"地址缺少";
    else
        user_t->address=str.toUtf8().data();
    str=ui->lineEdit_4->text();
    if(str==NULL)
    {
        QMessageBox::information(this,"注册失败","账号缺少");
        qDebug()<<"账号缺少";
        delete user_t;
        return;
    }
    else
        user_t->account=str.toUtf8().data();
    str=ui->lineEdit_5->text();
    if(str==NULL)
    {
        qDebug()<<"密码缺少";
        QMessageBox::information(this,"注册失败","密码缺少");
        delete user_t;
        return;
    }
    else
        user_t->password=str.toUtf8().data();
    str=ui->lineEdit_6->text();
    if(str==NULL)
    {
        qDebug()<<"确认密码缺少";
        QMessageBox::information(this,"注册失败","两次输入密码不一致");
        delete user_t;
        return;
    }
    else if(str!=ui->lineEdit_5->text())
    {
        qDebug()<<"两次输入密码不一致";
        QMessageBox::information(this,"注册失败","两次输入密码不一致");
        delete user_t;
        return;
    }
    else
        user_t->password=str.toUtf8().data();
    char* str1=struct2json(user_t,USER,1);
    qDebug()<<str1;
    delete user_t;

    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=insert_user_t;//登录，返回User
    int length=package_request(buf,str1,type);

    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;

    this->close();
    father->show();
    delete this;
}
