#include "mainwindow.h"
#include <QVector>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    layout = new QGridLayout();

    plot = new QCustomPlot();
    plot2 = new QCustomPlot();

    layout->addWidget(plot, 0, 0, 3, 1);
    layout->addWidget(plot2, 3, 0, 3, 1);

    QList<QSerialPortInfo> sp_list = QSerialPortInfo::availablePorts();

    qDebug() << sp_list[0].portName();
    
    kp = new QSlider(Qt::Vertical);
    ki = new QSlider(Qt::Vertical);
    kd = new QSlider(Qt::Vertical);
    sp = new QSlider(Qt::Vertical);
    kp->setMaximum(0);
    kp->setMinimum(-1500);
    ki->setMaximum(15000);
    ki->setMinimum(-15000);
    kd->setMaximum(3000);
    kd->setMinimum(-3000);
    sp->setMaximum(200);
    sp->setMinimum(-200);

    lcd_kp = new QLCDNumber(this);
    lcd_kd = new QLCDNumber(this);
    lcd_ki = new QLCDNumber(this);
    lcd_sp = new QLCDNumber(this);

    layout->addWidget(sp, 0, 1, 5, 1);
    layout->addWidget(kp, 0, 2, 5, 1);
    layout->addWidget(ki, 0, 3, 5, 1);
    layout->addWidget(kd, 0, 4, 5, 1);
    layout->addWidget(lcd_sp, 5, 1, 1, 1);
    layout->addWidget(lcd_kp, 5, 2, 1, 1);
    layout->addWidget(lcd_ki, 5, 3, 1, 1);
    layout->addWidget(lcd_kd, 5, 4, 1, 1);

    serial_thread = new QThread();
    //serialReader = new SerialComms("/dev/pts/2", 115200);
    serialReader = new SerialComms("/dev/" + sp_list[0].portName(), 115200);
    serialReader->moveToThread(serial_thread);
    connect(serialReader, SIGNAL(finished()), serial_thread, SLOT(quit()));
    //connect(serial_thread, SIGNAL(started()), serialReader, SLOT(readData()));
    connect(serial_thread, SIGNAL(started()), serialReader, SLOT(start()));
    connect(serialReader, SIGNAL(dataReady(QByteArray*)), this, SLOT(readData(QByteArray*)));
    //connect(QCoreApplication.instance(),SIGNAL()
    serial_thread->start();

    connect(kp, SIGNAL(valueChanged(int)), this, SLOT(setKp(int)));
    connect(ki, SIGNAL(valueChanged(int)), this, SLOT(setKi(int)));
    connect(kd, SIGNAL(valueChanged(int)), this, SLOT(setKd(int)));
    connect(sp, SIGNAL(valueChanged(int)), this, SLOT(setSp(int)));
    //connect(kp, SIGNAL(valueChanged(int)), this->lcd_kp, SLOT(display(int)));
    //connect(ki, SIGNAL(valueChanged(int)), this->lcd_ki, SLOT(display(int)));
    //connect(kd, SIGNAL(valueChanged(int)), this->lcd_kd, SLOT(display(int)));
    
    //connect(this, SIGNAL(updateKd(QByteArray)), serialReader, SLOT(write(const QByteArray)));
    //connect(this, SIGNAL(updateKp(QByteArray)), serialReader, SLOT(write(const QByteArray)));
    //connect(this, SIGNAL(updateKi(QByteArray)), serialReader, SLOT(write(const QByteArray)));
    //connect(this, SIGNAL(updateSp(QByteArray)), serialReader, SLOT(write(const QByteArray)));
    connect(this, SIGNAL(enviar(QByteArray)), serialReader, SLOT(write(const QByteArray)));

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
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setName("Duty Cycle: 0");
    //plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    //plot->graph(0)->setAntialiasedFill(false);
    //plot->graph(0)->setData(xdata, ydata);
    //plot->xAxis->setRange(-1, 1);
    plot->yAxis->setRange(-1050, 1050);
    plot->legend->setVisible(true);

    plot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    plot->xAxis->setDateTimeFormat("hh:mm:ss");
    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setTickStep(2);
    plot->axisRect()->setupFullAxesBox();

    plot2->addGraph();
    plot2->addGraph();
    plot2->graph(0)->setPen(QPen(Qt::red));
    plot2->graph(0)->setName("10*Tilt: 0 [deg]");
    plot2->graph(1)->setPen(QPen(Qt::blue));
    plot2->graph(1)->setName("D error: 0 [deg/s]");
    //plot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    //plot->graph(0)->setAntialiasedFill(false);
    //plot->graph(0)->setData(xdata, ydata);
    //plot->xAxis->setRange(-1, 1);
//    plot2->yAxis->setRange(-500, 500);
    plot2->yAxis->setRange(-260, 260);
    plot2->legend->setVisible(true);

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
    this->serialReader->write("t");
    this->serialReader->stop();
    //this->serial_thread->quit();
    //this->serial_thread->wait();
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

void MainWindow::readData(QByteArray * d)
{
    double time = datetime->currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double last_time;
    QByteArray m = d->trimmed();
    delete d;
    QList<QByteArray> list;
    bool ok;
    const int width = 5;
    float tilt = 0;
    float derror = 0;

    if( m.startsWith(':')){
        QList<QByteArray> l1 = m.split(':');
        list = l1[1].split('\t');
    }else{
		if (m == "Holaa")
			emit enviar("T");
        qDebug() << m;
        return;
    }
    
    if (list.length() >= 3){
        int16_t pwm = ((int16_t)list[0].toInt(&ok, 16));
        tilt = 10*list[2].toFloat();
        derror = list[1].toFloat();

        this->plot2->graph(0)->addData(time, tilt);
        this->plot2->graph(0)->removeDataBefore(time-width);
        this->plot2->graph(1)->addData(time, derror);
        this->plot2->graph(1)->removeDataBefore(time-width);

        this->plot->graph(0)->addData(time, pwm);

        this->plot->graph(0)->removeDataBefore(time-width);
        /*
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
        */
        this->plot2->xAxis->setRange(time+0.25, width, Qt::AlignRight);
        this->plot->xAxis->setRange(time+0.25, width, Qt::AlignRight);

        if( (time - last_time) > 0.05){
            this->plot->graph(0)->setName(QString("Duty Cycle: %1").arg(pwm, 5, 'd', 0, ' '));
            this->plot2->graph(0)->setName(QString("10*Tilt: %1 [deg]").arg(tilt, 7, 'f', 2, ' '));
            this->plot2->graph(1)->setName(QString("D error: %1 [deg/s]").arg(derror, 7, 'f', 2, ' '));
            this->plot->replot();
            this->plot2->replot();
            last_time = time;
        }
    }
}

void MainWindow::setKp(int kp)
{
    QString msg = QString("p:%1:").arg(kp, 4, 16);
    msg.remove(2, msg.length()-4-3);
    this->lcd_kp->display(kp);
    emit enviar(msg.toAscii());
}

void MainWindow::setKd(int kd)
{
    QString msg = QString("d:%1:").arg(kd, 4, 16 );
    msg.remove(2, msg.length()-4-3);
    this->lcd_kd->display(kd*0.001);
    emit enviar(msg.toAscii());
}

void MainWindow::setKi(int ki)
{
    QString msg = QString("i:%1:").arg(ki, 4, 16);
    msg.remove(2, msg.length()-4-3);
    this->lcd_ki->display(ki*0.01);
    emit enviar(msg.toAscii());
}

void MainWindow::setSp(int sp)
{
    QString msg = QString("s:%1:").arg(sp, 4, 16);
    msg.remove(2, msg.length()-4-3);
    this->lcd_sp->display(sp*0.01);
    emit enviar(msg.toAscii());
}
