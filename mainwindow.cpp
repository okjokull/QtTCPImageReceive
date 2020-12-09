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
     * The inferior stopped because it received a signal from the operating system. ;��ָ��
*/
    timer = NULL;
    qtcpserver = NULL;
    qtcpsocket = NULL;
    init();


}



void MainWindow::init()
{

    this->qtcpserver = new QTcpServer(this);
    this->qtcpsocket = new QTcpSocket(this); //�����׽���
    this->qtcpsocket->setProxy(QNetworkProxy::NoProxy);
    this->qtcpserver->setProxy(QNetworkProxy::NoProxy);

    this->timer = new QTimer();
   // this->timer->setSingleShot(true);��ʱһ��ˢ��




    newListen(); //���ڼ����µ�����
    connect(timer,SIGNAL(timeout()),this,SLOT(sendData()));
    connect(qtcpserver,SIGNAL(newConnection()),this,SLOT(acceptconnetion()));//��������
    connect(qtcpsocket,SIGNAL(error(QAbstractSocket::SocketError)/*������ʱ�򷢳��źţ������Ǵ�����������*/),
            this,SLOT(display(QAbstractSocket::SocketError)));
}
void MainWindow::acceptconnetion()
{
    qtcpsocket =qtcpserver->nextPendingConnection();//ָ��������һ�����ӵ��׽���
}
void MainWindow::display(QAbstractSocket::SocketError)
{
  qDebug()<<"occur:"<<qtcpsocket->errorString();
  qtcpsocket->close();
}
void MainWindow::newListen()
{

    //�����Ƿ��пͻ������ã��Ҷ��κ������߼�����
  if(!qtcpserver->listen(QHostAddress::Any,5555))
  {
      qDebug()<<"error :: "<<qtcpserver->errorString();//ʧ��
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
     QDataStream out(&Data,QIODevice::ReadWrite); // out������д�������룩��data�� <<
     displayImage();
     iMage->save(&buffer,"jpg");

     out.setVersion(QDataStream::Qt_5_6);//���л���ʽ���ð汾��
     out<<(quint32)buffer.data().size();
     Data.append(buffer.data());

     this->qtcpsocket->open(QTcpSocket::ReadWrite);
     this->qtcpsocket->write(Data);


     Data.resize(0);
     buffer.reset(); //�����ڴ�

}

void MainWindow:: displayImage()
{
    FILE *fp;
    char file[1024 * 10 * 10] = {0};
    char name[100] = {0};

    sprintf(name,"C:\\shiyan\\1.jpg",count ++); //ע����windows�µľ���·��Ҫ����д
    fp = fopen(name,"rb"); //�ɶ�д�Ķ������ļ� fb���ļ���ʶ����int��
    if(fp == NULL)
    {
        qDebug()<<"open failed ";
    }
    fread(&file,sizeof(file),1,fp);
    fclose(fp);
    qDebug()<<"count "<<count;

    if(count == 7) count = 1;
    iMage = new QImage((unsigned char *)file,0,0,QImage::Format_RGB16);
    bool isOk = iMage->loadFromData((unsigned char *)file,sizeof(file)); //���������Ƶĵ�һ��len����ͼ��
    qDebug()<<QString("isok: %1 ").arg(isOk);
    if (isOk)
    {

        ui->label->setScaledContents(true); //������п��ÿռ�
        ui->label->setPixmap(QPixmap::fromImage(*iMage,Qt::AutoColor)); //�ƶ��ı�־�滻��PIXMAP������
    }

}
MainWindow::~MainWindow()
{
    delete iMage;
    delete ui;
}
