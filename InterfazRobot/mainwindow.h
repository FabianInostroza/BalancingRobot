#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QDial>
#include <QGridLayout>
#include <QTimer>
#include <QDateTime>
#include <QVector>
#include <QLCDNumber>
#include "qcustomplot.h"
#include <QThread>
#include <QLCDNumber>
#include <QSlider>
#include "serialcomms.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void updatePlot();
    void readData(QByteArray *d);

private slots:
    void setKp(int kp);
    void setKd(int kd);
    void setKi(int ki);
private:
    QDial * dial;
    QTimer * timer;
    QDateTime * datetime;
    QVector<int> * xdata;
    QVector<float> * ydata;
    QGridLayout * layout;
    QCustomPlot * plot, * plot2;
    QLCDNumber * lcd;
    QThread * serial_thread;
    SerialComms * serialReader;
    QSlider * kp;
    QSlider * kd;
    QSlider * ki;
    QLCDNumber * lcd_kp;
    QLCDNumber * lcd_kd;
    QLCDNumber * lcd_ki;

signals:
    void updateKp(QByteArray s);
    void updateKd(QByteArray s);
    void updateKi(QByteArray s);

};

#endif // MAINWINDOW_H
