#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <qtimer.h>
#include <qdebug.h>
#include <qimage.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void init();
    void newListen();
private slots:
    void on_pushButton_clicked();
    void display(QAbstractSocket::SocketError);
    void acceptconnetion();
    void sendData();
    void displayImage();

private:
    Ui::MainWindow *ui;
    QTcpServer *qtcpserver;
    QTcpSocket *qtcpsocket;
    int send_flag;//发送次数的标志位
    QTimer *timer;
    QImage *iMage;
};

#endif // MAINWINDOW_H
