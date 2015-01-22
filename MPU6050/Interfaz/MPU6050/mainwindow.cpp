#include "mainwindow.h"
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout();

    plot = new QCustomPlot();
    plot2 = new QCustomPlot();

    layout->addWidget(plot, 0, 0);
    layout->addWidget(plot2, 1, 0);

    serial_thread = new QThread();
    serialReader = new SerialReaderWriter("/dev/ttyUSB0", 115200);
    connect(serialReader, SIGNAL(finished()), serial_thread, SLOT(quit()));
    //connect(serial_thread, SIGNAL(started()), serialReader, SLOT(readData()));
    serialReader->moveToThread(serial_thread);
    connect(serialReader, SIGNAL(dataReady(QByteArray)), this, SLOT(readData(QByteArray)));
    //connect(QCoreApplication.instance(),SIGNAL()
    serial_thread->start();

    // desactiva antialias
    /*
    plot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    plot->xAxis->setTickLabelFont(font);
    plot->yAxis->setTickLabelFont(font);
    plot->legend->setFont(font);
    */

//    xdata = new QVector<int> (101); // initialize with entries 0..100
//    ydata = new QVector<float> (101);
//    for (int i=0; i<101; ++i)
//    {
//      xdata->at(i) = i-50; // x goes from -1 to 1
//      ydata->at(i) = xdata->at(i)*xdata->at(i); // let's plot a quadratic function
//    }

    plot->addGraph(); // blue line
    plot->addGraph(); // blue line
    plot->addGraph(); // blue line
    plot->graph(0)->setPen(QPen(Qt::red));
    plot->graph(1)->setPen(QPen(Qt::green));
    plot->graph(2)->setPen(QPen(Qt::blue));
    //plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    //plot->graph(0)->setAntialiasedFill(false);
    //plot->graph(0)->setData(xdata, ydata);
    //plot->xAxis->setRange(-1, 1);
    plot->yAxis->setRange(-2, 2);

    plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plot->xAxis->setDateTimeFormat("hh:mm:ss");
    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setTickStep(2);
    plot->axisRect()->setupFullAxesBox();

    plot2->addGraph(); // blue line
    plot2->addGraph(); // blue line
    plot2->addGraph(); // blue line
    plot2->graph(0)->setPen(QPen(Qt::red));
    plot2->graph(1)->setPen(QPen(Qt::green));
    plot2->graph(2)->setPen(QPen(Qt::blue));
    //plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    //plot->graph(0)->setAntialiasedFill(false);
    //plot->graph(0)->setData(xdata, ydata);
    //plot->xAxis->setRange(-1, 1);
    plot2->yAxis->setRange(-500, 500);

    plot2->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plot2->xAxis->setDateTimeFormat("hh:mm:ss");
    plot2->xAxis->setAutoTickStep(false);
    plot2->xAxis->setTickStep(2);
    plot2->axisRect()->setupFullAxesBox();

    datetime = new QDateTime();

    this->setLayout(layout);
    this->resize(500, 300);
}

MainWindow::~MainWindow()
{
    qDebug("chao");
    this->serialReader->stop();
    this->serial_thread->quit();
    delete layout;
    delete plot;
    delete plot2;
    delete datetime;
}

void MainWindow::updatePlot()
{
    double time = datetime->currentDateTime().toMSecsSinceEpoch()/1000.0;
    this->lcd->display(this->dial->value());
    this->plot->graph(0)->addData(time, this->dial->value());
    this->plot->graph(0)->removeDataBefore(time-8);
    this->plot->xAxis->setRange(time+0.25, 8, Qt::AlignRight);
    this->plot->replot();
}

void MainWindow::readData(QByteArray d)
{
    double time = datetime->currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double last_time;
    QList<QByteArray> list = d.split(':');
    bool ok;
    //qDebug()  << list;
    if (list.length() >= 6){
        float ax = ((int16_t)list[0].toInt(&ok, 16))/16384.0;
        float ay = ((int16_t)list[1].toInt(&ok, 16))/16384.0;
        float az = ((int16_t)list[2].toInt(&ok, 16))/16384.0;

        float gx = ((int16_t)list[3].toInt(&ok, 16))/65.5;
        float gy = ((int16_t)list[4].toInt(&ok, 16))/65.5;
        float gz = ((int16_t)list[5].toInt(&ok, 16))/65.5;
    //    qDebug() << ax;
        this->plot->graph(0)->addData(time, ax);
        this->plot->graph(0)->removeDataBefore(time-8);
        this->plot->graph(1)->addData(time, ay);
        this->plot->graph(1)->removeDataBefore(time-8);
        this->plot->graph(2)->addData(time, az);
        this->plot->graph(2)->removeDataBefore(time-8);
        this->plot->xAxis->setRange(time+0.25, 8, Qt::AlignRight);

        this->plot2->graph(0)->addData(time, gx);
        this->plot2->graph(0)->removeDataBefore(time-8);
        this->plot2->graph(1)->addData(time, gy);
        this->plot2->graph(1)->removeDataBefore(time-8);
        this->plot2->graph(2)->addData(time, gz);
        this->plot2->graph(2)->removeDataBefore(time-8);
        this->plot2->xAxis->setRange(time+0.25, 8, Qt::AlignRight);
        if( (time - last_time) > 0.05){
            this->plot->replot();
            this->plot2->replot();
            last_time = time;
        }
    }
}
