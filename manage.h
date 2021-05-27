#ifndef MANAGE_H
#define MANAGE_H

#include <QWidget>
#include <QTcpSocket>
namespace Ui {
class manage;
}

class manage : public QWidget
{
    Q_OBJECT

public:
    explicit manage(QWidget *parent = nullptr);
    ~manage();
    QTcpSocket *tcpClient;

private:
    Ui::manage *ui;
};

#endif // MANAGE_H
