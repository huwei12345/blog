#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTreeWidgetItem>
#include "blog.h"
#include "addjson.h"
#include <QFontDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <qfiledialog.h>
#include <QInputDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setfather(QWidget* father);
    void update_group(Group* group,int count);
    void update_article_list(Article* article,int count);
    void update_article(Article* article);
    void update_comment(Comment* comment,int count);
    void insert_article(Article *article,int count=1);
    QTcpSocket *tcpClient;
    User* user;//可能访问其他用户的博客
    User* my_user;//标识用户自身
    int article_count;
    int comment_count;
    int group_now;
    void init_other_blog();
    void init_other_article();
    //设置关注、收藏开关的状态
    void set_rel_on();
    void set_col_on();
    void set_rel_down();
    void set_col_down();
    int get_group();
    void update_new_group(Group* group);
    void delete_group();
    void get_type_group(int* type,int *id);
    void modify_father_group();
    void modify_article_group();

private slots:
    void on_upload_triggered();

    void on_return_btn_triggered();
    void Double_click(QTreeWidgetItem *item,int);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_8_clicked();

    void on_action_7_triggered();

    void on_pushButton_9_clicked();

    void on_pushButton_7_clicked();

    void on_download_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_pushButton_10_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();

private:
    Ui::MainWindow *ui;
    QWidget* father;
    Group* group;
    Article* article;
    Comment* now_comment;
};
#endif // MAINWINDOW_H
