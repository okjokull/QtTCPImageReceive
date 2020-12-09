#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTextStream"
#include "QIODevice"
#include "qnetworkproxy.h"
#include "qdebug.h"
#include "QByteArray"
#include "qdatastream.h"
#include "qbuffer.h"
#include "stdio.h"


int count = 1 ;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

 //   QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);

    /*
     * The inferior stopped because it received a signal from the operating system. ;空指针
*/
    timer = NULL;
    qtcpserver = NULL;
    qtcpsocket = NULL;
    init();


}



void MainWindow::init()
{

    this->qtcpserver = new QTcpServer(this);
    this->qtcpsocket = new QTcpSocket(this); //创建套接字
    this->qtcpsocket->setProxy(QNetworkProxy::NoProxy);
    this->qtcpserver->setProxy(QNetworkProxy::NoProxy);

    this->timer = new QTimer();
   // this->timer->setSingleShot(true);定时一秒刷新




    newListen(); //用于监听新的连接
    connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));
    connect(qtcpserver,SIGNAL(newConnection()),this,SLOT(acceptconnetion()));//接受请求
    connect(qtcpsocket,SIGNAL(error(QAbstractSocket::SocketError)/*错误发生时候发出信号，参数是错误发生的类型*/),
            this,SLOT(display(QAbstractSocket::SocketError)));
}
void MainWindow::acceptconnetion()
{
    qtcpsocket =qtcpserver->nextPendingConnection();//指导返回下一个链接的套接字
}
void MainWindow::display(QAbstractSocket::SocketError)
{
  qDebug()<<"occur:"<<qtcpsocket->errorString();
  qtcpsocket->close();
}
void MainWindow::newListen()
{

    //监听是否有客户端来访，且对任何来访者监听，
  if(!qtcpserver->listen(QHostAddress::Any,5555))
  {
      qDebug()<<"error :: "<<qtcpserver->errorString();//失败
      close();
      return;
  }
}
void MainWindow::on_pushButton_clicked()
{
  //this->qtcpsocket->open(QTcpSocket::ReadWrite);
  // this->qtcpsocket->write(ui->lineEdit->text().toLatin1());

    this->timer->start(1000 * 2);

}

void MainWindow::sendData()
{
     QByteArray Data;
     QBuffer  buffer;
     buffer.open(QIODevice::ReadWrite);
     QDataStream out(&Data,QIODevice::ReadWrite); // out将数据写到（输入）到data中 <<
     displayImage();
     iMage->save(&buffer,"jpg");

     out.setVersion(QDataStream::Qt_5_6);//序列化格式设置版本号
     out<<(quint32)buffer.data().size();
     Data.append(buffer.data());

     this->qtcpsocket->open(QTcpSocket::ReadWrite);
     this->qtcpsocket->write(Data);


     Data.resize(0);
     buffer.reset(); //回收内存

}

void MainWindow:: displayImage()
{
    FILE *fp;
    char file[1024 * 10 * 10] = {0};
    char name[100] = {0};

    sprintf(name,"C:\\shiyan\\1.jpg",count ++); //注意在windows下的绝对路径要这样写
    fp = fopen(name,"rb"); //可读写的二进制文件 fb是文件标识符（int）
    if(fp == NULL)
    {
        qDebug()<<"open failed ";
    }
    fread(&file,sizeof(file),1,fp);
    fclose(fp);
    qDebug()<<"count "<<count;

    if(count == 7) count = 1;
    iMage = new QImage((unsigned char *)file,0,0,QImage::Format_RGB16);
    bool isOk = iMage->loadFromData((unsigned char *)file,sizeof(file)); //给定二进制的第一个len加载图像
    qDebug()<<QString("isok: %1 ").arg(isOk);
    if (isOk)
    {

        ui->label->setScaledContents(true); //填充所有可用空间
        ui->label->setPixmap(QPixmap::fromImage(*iMage,Qt::AutoColor)); //制定的标志替换了PIXMAP的数据
    }

}
MainWindow::~MainWindow()
{
    delete iMage;
    delete ui;
}
