#!/usr/bin/env python2

from PyQt4 import QtGui, QtCore
from serial.tools.list_ports import comports
import serial

class SerialPort(QtCore.QObject):
    finished = QtCore.pyqtSignal()
    received = QtCore.pyqtSignal([list])
    def __init__(self, ser):
        super(SerialPort, self).__init__()
        self._stop = False
        self._ser = ser

    def run(self):
        try:
            while not self._stop:
                msg = self._ser.readline()
                if len(msg) > 0:
                    datos = msg.split(':')
                    self.received.emit(datos)
            self._ser.close()
            self._stop = False
        except:
            print 'Error recibiendo'
        self.finished.emit()

    def send(self, d):
        self._ser.write(d)

    def stop(self):
        self._stop = True
        # while self._stop:
        #     pass

class MainWindow(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self._lay = QtGui.QGridLayout()

        coms = comports()

        try:
            #ser = Serial('/dev/ttyUSB0', 19200, timeout=0.1)
            print coms[-1][0]
            ser = serial.Serial(coms[-1][0], 38400, timeout=0.1)
            self.thread = QtCore.QThread()
            self.ser = SerialPort(ser)
            self.ser.finished.connect(self.thread.quit)
            self.ser.received.connect(self.updateData)
            self.ser.moveToThread(self.thread)
            self.thread.started.connect(self.ser.run)
            self.thread.start()
            #QtCore.QCoreApplication.instance().aboutToQuit.connect(self.thread.wait)
        except:
            print "Error conectando"

        self._lblAddr = QtGui.QLabel('Dir (hex)')
        self._addr = QtGui.QLineEdit(inputMask='HH')
        self._addr.setMaximumWidth(50)

        self._lblReg = QtGui.QLabel('Reg (hex)')
        self._reg = QtGui.QLineEdit(inputMask='HH')
        self._reg.setMaximumWidth(50)

        self._lblDat = QtGui.QLabel('Dato (hex)')
        self._dat = QtGui.QLineEdit(inputMask='HH')
        self._dat.setMaximumWidth(50)

        self._read = QtGui.QPushButton('Leer')
        self._read.clicked.connect(self.read)
        self._write = QtGui.QPushButton('Escribir')
        self._write.clicked.connect(self.write)

        self._poll = QtGui.QPushButton('Poll Addr')
        self._poll.clicked.connect(self.pollAddr)

        self._lay.addWidget(self._lblAddr, 0, 0)
        self._lay.addWidget(self._addr, 0, 1)

        self._lay.addWidget(self._lblReg, 1, 0)
        self._lay.addWidget(self._reg, 1, 1)

        self._lay.addWidget(self._lblDat, 2, 0)
        self._lay.addWidget(self._dat, 2, 1)

        self._lay.addWidget(self._read, 3, 0)
        self._lay.addWidget(self._write, 3, 1)
        self._lay.addWidget(self._poll, 3, 2)

        self.setLayout(self._lay)

    def getStrings(self):
        addr = self._addr.text()
        reg = self._reg.text()
        dat = self._dat.text()
        return {'addr': str(addr), 'reg': str(reg), 'dat': str(dat)}

    def read(self):
        s = self.getStrings()
        self.ser.send('R:'+s['addr']+':'+s['reg']+':\n')

    def write(self):
        s = self.getStrings()
        self.ser.send('W:'+s['addr']+':'+s['reg']+':'+s['dat']+':\n')

    def pollAddr(self):
        s = self.getStrings()
        self.ser.send('P:'+s['addr']+':\n')

    def updateData(self, d):
        print d
        if d[0] == 'R':
            self._dat.setText(d[3])

    def closeEvent(self, ev):
        try:
            self.ser.stop()
        except:
            pass
        print "Saliendo"


if __name__ == '__main__':
    app = QtGui.QApplication([])
    mw = MainWindow()
    mw.show()
    app.exec_()