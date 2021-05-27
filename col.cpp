#include "col.h"
#include "ui_col.h"

Col::Col(QWidget *parent) :
    QMainWindow(parent),
    user(NULL),
    ui(new Ui::Col),
    father(NULL),
    collect(NULL)
{
    ui->setupUi(this);
}

Col::~Col()
{
    delete ui;
}

void Col::on_pushButton_clicked()
{
    //如何返回，如何关闭，如何销毁 如何跳转
    father->show();
    this->close();
}
void Col::setfather(QWidget* father)
{
    this->father=father;
}

void Col::set_col(Collect *collect)
{
    if(this->collect!=NULL)
        delete this->collect;
    this->collect=collect;
}

void Col::update(Collect *collect, int count)
{
    QStringList str;
    set_col(collect);
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"文章id"<<"作者id"<<"文章名");
    ui->tableWidget->setRowCount(count);
    for(int j=0;j<count;j++)
    {
        ui->tableWidget->setItem(j,0,new QTableWidgetItem(QString::number(collect[j].collect_art_id)));
        ui->tableWidget->setItem(j,1,new QTableWidgetItem(QString::number(collect[j].art_user_id)));
        ui->tableWidget->setItem(j,2,new QTableWidgetItem(collect[j].art_name));
    }
    if(collect!=NULL)
        delete[] collect;
}

/*
void Col::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    mainwindow->user=new User;
    if(item->)

    //mainwindow->user->user_id;

    mainwindow->tcpClient=tcpClient;
    mainwindow->setfather(this);

    //buffer 状态 请求类型 json的对象引用，数目，对象类型
    char *buf=new char[500];
    memset(buf,'\0',500);
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
    //查组，查文章名，查文章。返回直接到new，删除mainwindow，再点博客，回到自己的博客
    //做一个保存文章功能，添加评论功能。
    //关注、收藏同理
    //然后再写关注、点赞功能。另外注意视图和数据的关系。
}
*/

void Col::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item->column()==1)//user_id
    {
        mainwindow->user=new User;
        int num=item->text().toInt();
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
}
