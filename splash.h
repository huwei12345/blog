#ifndef SPLASH_H
#define SPLASH_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include "QMessageBox"
#include <QDebug>
#include <QTime>
#include "tcp_net.h"
#include "addjson.h"
#include "register.h"
namespace Ui {
class splash;
}

class splash : public QWidget
{
    Q_OBJECT

public:
    explicit splash(QWidget *parent = nullptr);
    ~splash();
    void login(QString username,QString password);
    Tcp_net* tcp;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

private:
    Ui::splash *ui;
};

#endif // SPLASH_H
