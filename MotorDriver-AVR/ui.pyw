#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from serial import Serial

class MainWindow(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)

        self._lay = QtGui.QGridLayout(self)

        self.setLayout(self._lay)

        self.ser = Serial('/dev/ttyUSB0', 38400)

        self._dialA = QtGui.QDial(minimum=-0x3ff/2+1, maximum=0x3ff/2)
        self._dialB = QtGui.QDial(minimum=-0x3ff/2+1, maximum=0x3ff/2)

        self._lcdA = QtGui.QLCDNumber(self)
        self._lcdB = QtGui.QLCDNumber(self)

        self._dialA.valueChanged.connect(self._lcdA.display)
        self._dialA.valueChanged.connect(self.setMotorASpeed)
        self._dialB.valueChanged.connect(self._lcdB.display)
        self._dialB.valueChanged.connect(self.setMotorBSpeed)

        self.setMotorASpeed(0)
        self.setMotorBSpeed(0)

        self._lay.addWidget(self._dialA, 0, 0)
        self._lay.addWidget(self._dialB, 0, 1)
        self._lay.addWidget(self._lcdA, 1, 0)
        self._lay.addWidget(self._lcdB, 1, 1)

        self.resize(500, 300)

    def setMotorASpeed(self, v):
        #print 'A:%X'%(v+0x1ff)
        self.ser.write('A:%X\n'%(v+0x1ff))

    def setMotorBSpeed(self, v):
        #print 'B:%X'%(v+0x1ff)
        self.ser.write('B:%X\n'%(v+0x1ff))

if __name__ == '__main__':
    app = QtGui.QApplication([])
    mw = MainWindow()
    mw.show()
    app.exec_()