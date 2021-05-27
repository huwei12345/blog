#ifndef NEW_H
#define NEW_H

#include <QWidget>
#include "mainwindow.h"
#include "focus.h"
#include "col.h"
#include "forum.h"
#include "form.h"
#include "blog.h"
#include "addjson.h"
namespace Ui {
class New;
}

class New : public QWidget
{
    Q_OBJECT

public:
    explicit New(QWidget *parent = nullptr);
    ~New();
    void set_User(User* user);
    void init();
    QTcpSocket *tcpClient;
    void setfather(QWidget* father);
    QWidget* father;
    focus* focus;
    Col* col;
    forum* forum;
    Form* form;
    Search* search;
    MainWindow* mainwindow;
    User* user;//标识用户自身

private slots:
    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::New *ui;


};

#endif // NEW_H
