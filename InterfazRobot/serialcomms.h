#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

class SerialComms : public QObject
{
    Q_OBJECT
public:
    explicit SerialComms(QObject *parent = 0);
    explicit SerialComms(QString sp, qint32 baud, QObject *parent = 0);
    void setPortName(QString);
    void open(QString, qint32);
    int open();

signals:
    void dataReady(QByteArray * data);
    void finished();

public slots:
    void stop();
    void start(void);

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

#endif // SERIALCOMMS_H
