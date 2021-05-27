#ifndef FOCUS_H
#define FOCUS_H

#include <QWidget>
#include <QTcpSocket>
#include <QTableWidgetItem>
#include "blog.h"
#include "mainwindow.h"
namespace Ui {
class focus;
}

class focus : public QWidget
{
    Q_OBJECT

public:
    explicit focus(QWidget *parent = nullptr);
    ~focus();
    void setfather(QWidget* father);
    void update(User_Relation* rel,int count);
    QTcpSocket *tcpClient;
    User* user;
    MainWindow* mainwindow;
private slots:
    void on_pushButton_clicked();


//    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::focus *ui;
    QWidget* father;
    User_Relation* rel;
};

#endif // FOCUS_H
