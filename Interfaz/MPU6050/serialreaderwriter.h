#ifndef SERIALREADERWRITER_H
#define SERIALREADERWRITER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialReaderWriter : public QObject
{
    Q_OBJECT
public:
    explicit SerialReaderWriter(QObject *parent = 0);
    explicit SerialReaderWriter(QString sp, qint32 baud, QObject *parent = 0);
    void setPortName(QString);
    void start(void);
    int open(QString, qint32);
    int open();

    void stop();
signals:
    void dataReady(QByteArray data);
    void finished();

public slots:

protected slots:
    void readLine();
private:
    QSerialPort * m_serialPort;
    char nStop;
    QString portName;
    qint32 baudRate;

private slots:
    void readData();

};

#endif // SERIALREADERWRITER_H
