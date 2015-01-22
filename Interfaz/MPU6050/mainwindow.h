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
#include "serialreaderwriter.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void updatePlot();
    void readData(QByteArray d);

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
    SerialReaderWriter * serialReader;
};

#endif // MAINWINDOW_H
