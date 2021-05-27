#ifndef FORUM_H
#define FORUM_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidgetItem>
#include <QMessageBox>
#include "mainwindow.h"
#include "blog.h"
#include "search.h"
namespace Ui {
class forum;
}

class forum : public QWidget
{
    Q_OBJECT

public:
    explicit forum(QWidget *parent = nullptr);
    ~forum();
    void setfather(QWidget* father);
    QTcpSocket *tcpClient;
    User* user;
    MainWindow* mainwindow;
    void update_article_bytype(Article* article,int count);
    void update_article_bynow(Article* article,int count);
    void update_article_byweek(Article* article,int count);
    void update_article_bymonth(Article* article,int count);
    int get_user_id(QListWidgetItem *item);
    Search* search;
private slots:
    void on_pushButton_6_clicked();


    void on_pushButton_5_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_4_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_8_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_7_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_9_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_5_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_6_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_3_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_2_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::forum *ui;
    QWidget* father;
};

#endif // FORUM_H
