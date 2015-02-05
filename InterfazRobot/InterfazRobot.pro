#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T23:42:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = InterfazRobot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../Librerias/qcustomplot/qcustomplot.cpp \
    serialcomms.cpp

HEADERS  += mainwindow.h \
    ../Librerias/qcustomplot/qcustomplot.h \
    serialcomms.h

INCLUDEPATH += ../Librerias/qtserialport-build/include
LIBS += -L../Librerias/qtserialport-build/src/serialport -lQtSerialPort -ludev

#FORMS    +=
