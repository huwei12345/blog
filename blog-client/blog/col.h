#ifndef COL_H
#define COL_H

#include <QMainWindow>
#include <QTcpSocket>
#include<QTableWidgetItem>
#include "blog.h"
#include "mainwindow.h"
namespace Ui {
class Col;
}

class Col : public QMainWindow
{
    Q_OBJECT

public:
    explicit Col(QWidget *parent = nullptr);
    ~Col();
    void setfather(QWidget* father);
    void set_col(Collect* collect);
    QTcpSocket *tcpClient;
    void update(Collect* collect,int count);
    User* user;
    MainWindow* mainwindow;

private slots:
    void on_pushButton_clicked();

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::Col *ui;
    QWidget* father;
    Collect* collect;
};

#endif // COL_H
