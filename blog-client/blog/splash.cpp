#include "splash.h"
#include "ui_splash.h"
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//
splash::splash(QWidget *parent) :
    QWidget(parent),
    tcp(NULL),
    ui(new Ui::splash)
{
    ui->setupUi(this);
    this->setFixedSize(588,368);
    tcp=new Tcp_net;
    tcp->setfather(this);
    //connect(tcp->tcpClient,SIGNAL(readyRead()),this,SLOT(Tcp_net::ReadData()));
    //serv_fd=connect(serv_fd,serv_addr,sizeof(serv_addr));//或者类数据成员
}

splash::~splash()
{
    delete tcp;
    delete ui;
    qDebug()<<"splash 析构";
}


void splash::login(QString username,QString password)
{
    //检查
    if(username==NULL||username=="")
    {
        QMessageBox::information(this,"登陆失败","用户名不能为空");
        return;
    }
    if(password==NULL||password=="")
    {
        QMessageBox::information(this,"登录失败","密码不能为空");
        return;
    }
    //联网发送
    User* user=new User;
    user->account=username.toUtf8().data();
    user->password=password.toUtf8().data();
    char* str=struct2json(user,USER,1);
    delete user;
    char *buf=new char[500];
    memset(buf,'\0',500);
    short type=login_t;//登录，返回User
    int length=package_request(buf,str,type);
    //发送
    QTcpSocket* tcpClient=tcp->tcpClient;
    tcpClient->write(buf,length); //qt5去除了.toAscii()
    tcpClient->flush();
    delete[] buf;
}
//登录按钮
void splash::on_pushButton_clicked()
{
    QString username=this->ui->lineEdit->text();
    QString password=this->ui->lineEdit_2->text();
    login(username,password);//package+send
    //char* response =get_response();//响应只能在readyRead槽函数中获取了
}

void splash::on_pushButton_2_clicked()
{
    //注册
    Register* win=new Register;
    win->tcpClient=tcp->tcpClient;
    win->show();
    win->setfather(this);
    this->hide();
}

void splash::on_pushButton_3_clicked()
{
    //退出
    this->close();
}

void splash::on_pushButton_4_clicked()
{
    //忘记密码
    QMessageBox::information(this,"忘记密码","请联系管理员vx:xxxxxx");
}
