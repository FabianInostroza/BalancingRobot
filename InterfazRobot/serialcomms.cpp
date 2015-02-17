#include "serialcomms.h"
#include <QDebug>

SerialComms::SerialComms(QObject *parent) :
    QObject(parent)
{
    nStop = 1;
}

SerialComms::SerialComms(QString sp, qint32 baud, QObject *parent) :
    QObject(parent)
{
    portName = QString(sp);
    baudRate = baud;
    //this->m_serialPort = new QSerialPort();
    //this->m_serialPort->setPortName(this->portName);
    //this->m_serialPort->setBaudRate(this->baudRate);

    this->portName = sp;
    this->baudRate = baud;

    //this->m_serialPort->open(QIODevice::ReadWrite);
    //connect(this->m_serialPort, SIGNAL(readyRead()), this, SLOT(readLine()));
    nStop = 1;
}

SerialComms::~SerialComms()
{
	if (this->m_serialPort){
		if( this->m_serialPort->isOpen() )
			this->m_serialPort->close();
		delete this->m_serialPort;
	}
}

void SerialComms::setPortName(QString portName)
{
    //this->m_serialPort.setPortName(portName);
    this->portName = portName;
}

void SerialComms::open(QString portName, qint32 baud)
{
    this->baudRate = baud;
    this->portName = portName;
    //return this->m_serialPort.open(QIODevice::ReadWrite);
}

int SerialComms::open()
{
    return this->m_serialPort->open(QIODevice::ReadWrite);
}

void SerialComms::readData()
{
    while( this->m_serialPort->canReadLine() ){
        emit dataReady(new QByteArray(this->m_serialPort->readLine()));
    }

//    this->m_serialPort = new QSerialPort();
//    this->m_serialPort->setPortName(this->portName);
//    this->m_serialPort->setBaudRate(this->baudRate);

//    if (this->m_serialPort->open(QIODevice::ReadWrite)){
//        while(nStop){
//            if(this->m_serialPort->canReadLine()){
//                QByteArray data = this->m_serialPort->readLine();
//                emit dataReady(new QByteArray(data));
//            }
//        }
//    }


    //qDebug("chao2");
    //delete this->m_serialPort;
    //emit finished();
}

void SerialComms::start()
{
    this->m_serialPort = new QSerialPort();
    this->m_serialPort->setPortName(this->portName);
    this->m_serialPort->setBaudRate(this->baudRate);
    this->m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if (this->m_serialPort->open(QIODevice::ReadWrite)) {
        this->m_serialPort->flush();
        connect(this->m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    }else{
		qDebug() << "No se pudo abrir el puerto " + this->portName ;
	}
//    if (this->m_serialPort->open(QIODevice::ReadWrite)){
//        while(this->nStop){
//            if( this->m_serialPort->canReadLine()){
//                QByteArray * data = new QByteArray(this->m_serialPort->readLine());
//                emit dataReady(data);
//            }
//        }
//    }
}

void SerialComms::stop()
{
    disconnect(this->m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    //nStop = 0;
}

void SerialComms::readLine()
{

    while(this->m_serialPort->canReadLine()){
        QByteArray data = this->m_serialPort->readLine();
        emit dataReady(new QByteArray(data));
        //qDebug() << data << endl;
    }
}

void SerialComms::write(const char * str)
{
    if (this->m_serialPort && this->m_serialPort->isOpen()){
        this->m_serialPort->write(str);
    }
}

void SerialComms::write(const QByteArray str)
{
    if (this->m_serialPort && this->m_serialPort->isOpen()){
        this->m_serialPort->write(str);
    }
}
