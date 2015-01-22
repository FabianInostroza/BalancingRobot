#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
import serial
import sys
import pyqtgraph as pg
import numpy as np

com_port = "COM3" #"/dev/ttyUSB0"
com_port = "/dev/ttyUSB0"

class TimeAxisItem(pg.AxisItem):
    def __init__(self, *args, **kwargs):
        super(TimeAxisItem,self).__init__(*args, **kwargs)
        self._time = QtCore.QTime()

    def tickStrings(self, values, scale, spacing):
        labels = [self._time.addMSecs(value).toString('mm:ss') for value in values]
        return labels

    def tickSpacinggh(self, minVal, maxVal, size):
        return [(size/2.0, 0),
                (size/5.0, 0),
               ]

class Plot(pg.PlotWidget):
    def __init__(self,label='', *args, **kwargs):
        super(Plot,self).__init__(*args,
                                  axisItems={'bottom': 
                                             TimeAxisItem(orientation='bottom')},
                                  **kwargs)

        self.setMouseEnabled(x=False,y=False)
        self._curves = []
        self._xdatas = []
        self._ydatas = []
        self._nsamples = []
        #self.addLegend()
        self.addCurve(name=label)
        #self.showAxis('right')
        
        #self.setDownsampling(mode='peak')
        
        if 'xrange' in kwargs:
            self._xrange=kwargs['xrange']
        else:
            self._xrange=(0, 10000)
        
        self._plotlen = 100
        self._nsamples = [0]
        self.setXRange(*self._xrange)

    def wheelEvent(self,ev):
        pass

    def addCurve(self,x=[0],y=[0],pen=pg.mkPen('b'),**kwargs):
        #curve = pg.PlotDataItem(x,y,name=label,pen=pen)
        curve = self.plot(x,y,pen=pen,**kwargs)
        self._xdatas.append(np.zeros((100,), dtype='uint32'))
        self._ydatas.append(np.zeros((100,), dtype='float16'))
        self._nsamples.append(0)
        self._curves.append(curve)
        #curve = pg.PlotDataItem(x,y,pen=pen)
        #self._gr.addItem(curve)
        return curve
        
    def updatePlot(self, time, val, curve=0, upd=True):
        # si numero de muestras es mayor a tamaño 
        # del buffer, entonces desplazar los datos
        if self._nsamples[curve] >= 100:
            self._xdatas[curve] = np.roll(self._xdatas[curve], -1)
            self._ydatas[curve] = np.roll(self._ydatas[curve], -1)
            self._ydatas[curve][-1] = val
            self._xdatas[curve][-1] = time
        else:
            self._ydatas[curve][self._nsamples[curve]] = val
            self._xdatas[curve][self._nsamples[curve]] = time
            self._nsamples[curve] += 1
            
        # si se debe graficar...
        if upd:
            if self._nsamples[curve] >= 100:
                self._curves[curve].setData(self._xdatas[curve], 
                                            self._ydatas[curve])
            else:
                self._curves[curve].setData(self._xdatas[curve][:self._nsamples[curve]],
                                    self._ydatas[curve][:self._nsamples[curve]])
            # si el tiempo es mayor a 10 s desplazar el 
            # el eje x
            if time > 10:
                start = max([s[0] for s in self._xdatas])
                try:
                    self.setXRange(start, time)
                except Exception as ex:
                    print ex
                        
    def resetPlot(self):
        self._nsamples=0
        self.setXRange(*self._xrange)
        self._curve.clear()
        
    def setPlotLength(self, l):
        self._plotlen = l
        if len(self._xdata) > 0:
            self.setXRange(self._xdata[0],self._xdata[0]+l)
        else:
            self.setXRange(0,l)    

class COMM(QtCore.QThread):
    updateData = QtCore.pyqtSignal([list])
    def __init__(self, **kwargs):
        super(COMM, self).__init__(**kwargs)
        self._lock = QtCore.QReadWriteLock()
        self._stop = False
        self.DEBUG = False
        self.s = None

    def start(self, comport, baud=9600):
        self._stop = False
        self.comport = comport
        self.baud = baud
        super(COMM,self).start()
        
    def setPort(self, port, baud=9600):
        self.comport = port
        self.baud = baud
        if not self._stop:
            self.detener()
            
    def enableDebug(self, en):
        self.DEBUG = en

    def run(self):
        self.s = serial.Serial(self.comport, self.baud, timeout=0.1)
        while True:
            msg = self.s.readline()
            if len(msg) > 0:
                #print msg
                msg = msg.strip('\n')
                msg = msg.strip('\r')
                d = msg.split(':')
                if len(d) >= 6:
                    self.updateData.emit(d)

            if self._stop:
                break
            #sleep(0.1) # magia
        self.s.close()

    def write(self, l):
        if self.s and self.s.isOpen():
            if type(l) is list:
                for c in l:
                    if c is str:
                        self.s.write(c)
                    else:
                        self.s.write(chr(c))
            elif type(l) is str:
                self.s.write(l)
            else:
                self.s.write(chr(l))
            
    
    def detener(self):
        self._lock.lockForWrite()
        self._stop = True
        self._lock.unlock()

class MainWindow(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self._layout = QtGui.QGridLayout()
        self.comms = COMM()
        self.comms.updateData.connect(self.updatePlot)
        #self.comms.start('/dev/ttyUSB0', 9600)
        self.comms.start(com_port, 115200)

        self.time = QtCore.QTime()

        self.plotAx = Plot(title='ax')
        self.plotAx.setYRange(-2, 2)
        self.plotAy = Plot(title='ay')
        self.plotAy.setYRange(-2, 2)
        self.plotAz = Plot(title='az')
        self.plotAz.setYRange(-2, 2)
        
        self.plotGx = Plot(title='gx')
        self.plotGx.setYRange(-250, 250)
        self.plotGy = Plot(title='gy')
        self.plotGy.setYRange(-250, 250)
        self.plotGz = Plot(title='gz')
        self.plotGz.setYRange(-250, 250)

        self._layout.addWidget(self.plotAx, 0, 0)
        self._layout.addWidget(self.plotAy, 1, 0)
        self._layout.addWidget(self.plotAz, 2, 0)

        self._layout.addWidget(self.plotGx, 0, 1)
        self._layout.addWidget(self.plotGy, 1, 1)
        self._layout.addWidget(self.plotGz, 2, 1)

        self.setLayout(self._layout)
        self._sample_cnt = 0
        self._lastTime = 0

    def updatePlot(self, d):
        time = self.time.elapsed()
        try:
            ax = int(d[0],16)
            ay = int(d[1],16)
            az = int(d[2],16)
            gx = int(d[3],16)
            gy = int(d[4],16)
            gz = int(d[5],16)

            ax = (ax-(ax&0x8000)*2)/16384.0
            ay = (ay-(ay&0x8000)*2)/16384.0
            az = (az-(az&0x8000)*2)/16384.0

            gx = (gx-(gx&0x8000)*2)/131.0
            gy = (gy-(gy&0x8000)*2)/131.0
            gz = (gz-(gz&0x8000)*2)/131.0

            if (time - self._lastTime) >= 80:
                upd = True
                self._lastTime = time
            else:
                upd = False
            self.plotAx.updatePlot(time, ax, upd = upd)
            self.plotAy.updatePlot(time, ay, upd = upd)
            self.plotAz.updatePlot(time, az, upd = upd)

            self.plotGx.updatePlot(time, gx, upd = upd)
            self.plotGy.updatePlot(time, gy, upd = upd)
            self.plotGz.updatePlot(time, gz, upd = upd)
        except:
            print "error"


    def closeEvent(self, ev):
        self.comms.detener()
        super(MainWindow, self).closeEvent(ev)

if __name__ == '__main__':
    app = QtGui.QApplication([])
    mw = MainWindow()
    mw.show()
    app.exec_()