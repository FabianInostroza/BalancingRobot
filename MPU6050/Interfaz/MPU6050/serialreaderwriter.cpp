#include "serialreaderwriter.h"
#include <QDebug>

SerialReaderWriter::SerialReaderWriter(QObject *parent) :
    QObject(parent)
{
    nStop = 1;
}

SerialReaderWriter::SerialReaderWriter(QString sp, qint32 baud, QObject *parent) :
    QObject(parent)
{
    portName = QString(sp);
    baudRate = baud;
    this->m_serialPort = new QSerialPort();
    this->m_serialPort->setPortName(this->portName);
    this->m_serialPort->setBaudRate(this->baudRate);
    this->m_serialPort->open(QIODevice::ReadWrite);
    connect(this->m_serialPort, SIGNAL(readyRead()), this, SLOT(readLine()));
    nStop = 1;
}

void SerialReaderWriter::setPortName(QString portName)
{
    //this->m_serialPort.setPortName(portName);
    this->portName = portName;
}

int SerialReaderWriter::open(QString portName, qint32 baud)
{
    baudRate = baud;
    this->portName = portName;
    //return this->m_serialPort.open(QIODevice::ReadWrite);
}

int SerialReaderWriter::open()
{
    return this->m_serialPort->open(QIODevice::ReadWrite);
}

void SerialReaderWriter::readData()
{
//    while( this->m_serialPort.canReadLine() ){
//        emit dataReady(this->m_serialPort.readLine());
//    }
    this->m_serialPort = new QSerialPort();
    this->m_serialPort->setPortName(this->portName);
    this->m_serialPort->setBaudRate(this->baudRate);
    qDebug() << portName;
    qDebug() << baudRate;
    if (this->m_serialPort->open(QIODevice::ReadWrite)){
        while(nStop){
            if(this->m_serialPort->canReadLine()){
                qDebug() << "ada";
                QByteArray data = this->m_serialPort->readLine();
                emit dataReady(data);
//                qDebug() <<  << endl;
            }
        }
    }
    qDebug("chao2");
    delete this->m_serialPort;
    emit finished();
}

void SerialReaderWriter::start()
{
    connect(this->m_serialPort, SIGNAL(readyRead()), SLOT(readData()));
}

void SerialReaderWriter::stop()
{
    //disconnect(&this->m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
    nStop = 0;
}

void SerialReaderWriter::readLine()
{
    while(this->m_serialPort->canReadLine()){
        QByteArray data = this->m_serialPort->readLine();
        emit dataReady(data);
        //qDebug() << data << endl;
    }
}
