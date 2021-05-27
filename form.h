#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "blog.h"
#include <QTcpSocket>
#include "addjson.h"
#include <QInputDialog>
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(User* user=NULL,QWidget *parent = nullptr);
    ~Form();
    void setfather(QWidget* father);
    void set_User(User* user);
    QTcpSocket *tcpClient;
    User* user;
    void update();
private slots:
    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::Form *ui;
    QWidget* father;
};

#endif // FORM_H
