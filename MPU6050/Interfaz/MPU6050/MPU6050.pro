#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T23:42:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MPU6050
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    serialreaderwriter.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    serialreaderwriter.h

INCLUDEPATH += ../qtserialport-build/include
LIBS += -L../qtserialport-build/src/serialport -lQtSerialPort

#FORMS    +=
