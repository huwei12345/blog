#include "form.h"
#include "ui_form.h"
#include <QMessageBox>
Form::Form(User* user,QWidget *parent) :
    QWidget(parent),
    user(user),
    ui(new Ui::Form)
{
    ui->setupUi(this);
}
Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_clicked()
{
    //返回按钮
    father->show();
    this->close();
}

void Form::setfather(QWidget *father)
{
    this->father=father;
}

void Form::set_User(User *user)
{
    this->user=user;
}

void Form::update()
{
    if(user==NULL)
        QMessageBox::information(this,"无账号信息","无账号信息");
    else
    {
        if(user->user_id!=-1)
            ui->user_id_l->setText(QString::number(user->user_id));
        if(user->name!=NULL)
            ui->user_name_l->setText(user->name);
        if(user->address!=NULL)
            ui->user_add_l->setText(user->address);
        if(user->sex!=NULL)
            ui->user_sex_l->setText(user->sex);//gbk转utf8
        if(user->fans_num!=-1)
            ui->user_fans_l->setText(QString::number(user->fans_num));
        if(user->article_num!=-1)
            ui->user_art_l->setText(QString::number(user->article_num));
        if(user->create_time!=NULL)
            ui->user_ct_l->setText(user->create_time);
    }
}

void Form::on_pushButton_2_clicked()
{
    if(ui->user_sex_l->text()==NULL)
    {
        QMessageBox::information(this,"修改失败","性别不能为空");
        return;
    }
    if(ui->user_name_l->text()==NULL)
    {
        QMessageBox::information(this,"修改失败","昵称不能为空");
        return;
    }
    if(ui->user_add_l->text()==NULL)
    {
        QMessageBox::information(this,"修改失败","地址不能为空");
        return;
    }
    //修改
    User* user=new User;
    int len1,len2,len3;
    len1=ui->user_sex_l->text().length()+1;
    len2=ui->user_name_l->text().length()+1;
    len3=ui->user_add_l->text().length()+1;
    char* sex=new char[len1];
    char* us=new char[len2];
    char* add=new char[len3];
    strcpy(sex,ui->user_sex_l->text().toUtf8().data());
    strcpy(us,ui->user_name_l->text().toUtf8().data());
    strcpy(add,ui->user_add_l->text().toUtf8().data());

    user->sex=sex;
    user->name=us;
    user->address=add;
    user->user_id=this->user->user_id;
    //buffer 状态 请求类型 json的对象引用，数目，对象类型
    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=modify_user_t;//登录，返回User
    char* str=NULL;
    //如果有对象数据要传输
    str=struct2json(user,USER,1);
    qDebug()<<str;
    int length=package_request(buf,str,type);
    //发送
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->waitForBytesWritten();
    tcpClient->flush();
    delete[] buf;
    delete user;
    delete[] add;
    delete[] us;
    delete[] sex;
}

void Form::on_pushButton_3_clicked()
{
    //删除账户
    QString dlgTitle="删除账户";
    QString txtLabel="请输入：我想要删除我的账户";
    QString defaultInput=" ";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;//正常文字输入
//    QLineEdit::EchoMode echoMode=QLineEdit::Password;//密码输入

    bool ok=false;
    QString text = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
    if (ok && text=="我想要删除我的账户")
   {

        User* user=new User;
        user->user_id=this->user->user_id;

        //buffer 状态 请求类型 json的对象引用，数目，对象类型
        char *buf=new char[500];
        memset(buf,'\0',500);
        short type=delete_user_t;//登录，返回User
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
}
