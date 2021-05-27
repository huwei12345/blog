#ifndef TCP_NET_H
#define TCP_NET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include "mainwindow.h"
#include "focus.h"
#include "col.h"
#include "forum.h"
#include "form.h"
#include "new.h"
#include "form.h"
#include <QDebug>
#include "qdialog.h"
#include "QMessageBox"
#include <QDebug>
#include <QTime>
#include "addjson.h"
#include "search.h"
#define trans4(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))|((x[2]<<16)&(0x00ff0000))|((x[3]<<24)&(0xff000000))//
#define trans2(x) ((x[0])&(0x000000ff))|((x[1]<<8)&(0x0000ff00))//

namespace Ui {
class Tcp_net;
}

class Tcp_net : public QWidget
{
    Q_OBJECT

public:
    explicit Tcp_net(QWidget *parent = nullptr);
    ~Tcp_net();
    QTcpSocket *tcpClient;
    Col* col;
    focus *focus;
    forum *forum;
    MainWindow *mainwindow;
    Form* form;
    New *new_win;
    Search* search;
    int length;//包长度
    int len;//当前buffer中读道的数据
    static const int BUFFER_SIZE=1024*1024;//128M
    void setfather(QWidget* father);
    QWidget* father;
    char* m_buffer;
    void process_packet(char* buffer,int length);
private slots:
    void ReadData();

private:
    Ui::Tcp_net *ui;
};

#endif // TCP_NET_H
