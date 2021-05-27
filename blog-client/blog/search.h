#ifndef SEARCH_H
#define SEARCH_H

#include <QWidget>
#include "mainwindow.h"
#include <QListWidgetItem>
namespace Ui {
class Search;
}

class Search : public QWidget
{
    Q_OBJECT

public:
    explicit Search(QWidget *parent = nullptr);
    ~Search();
    void setfather(QWidget* father);
    MainWindow* mainwindow;
    void show_art_id(Article* article,int count);
    void show_art_title(Article* article,int count);
    void show_user_id(User* user,int count);
    void show_user_name(User* user,int count);
    int now_type;
    QTcpSocket* tcpClient;
private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Search *ui;
    QWidget* father;
};

#endif // SEARCH_H
