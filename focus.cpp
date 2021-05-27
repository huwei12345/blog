#include "focus.h"
#include "ui_focus.h"
#include <qlistwidget.h>
focus::focus(QWidget *parent) :
    QWidget(parent),
    user(NULL),
    ui(new Ui::focus),
    rel(NULL)
{
    ui->setupUi(this);
}

focus::~focus()
{
    delete ui;
}

void focus::on_pushButton_clicked()
{
    //返回按钮
    father->show();
    this->close();
}

void focus::setfather(QWidget *father)
{
    this->father=father;
}


void focus::update(User_Relation *rel,int count)
{
    QStringList str;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(2);
    //table
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"用户id"<<"用户名");
    ui->tableWidget->setRowCount(count);
    for(int j=0;j<count;j++)
    {
        ui->tableWidget->setItem(j,0,new QTableWidgetItem(QString::number(rel[j].rel_user_id)));
        ui->tableWidget->setItem(j,1,new QTableWidgetItem(rel[j].name));
    }
    //试试
    if(rel!=NULL)
        delete[] rel;
}


void focus::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item->column()==0)
    {
        mainwindow->user=new User;
        int num=item->text().toInt();
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
