#!/usr/bin/env python2
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
from matplotlib.mlab import csv2rec
from numpy import arctan2 as atan2
from numpy import pi
from numpy import zeros

# These are the "Tableau 20" colors as RGB.  
tableau20 = [(31, 119, 180), (174, 199, 232), (255, 127, 14), (255, 187, 120),  
             (44, 160, 44), (152, 223, 138), (214, 39, 40), (255, 152, 150),  
             (148, 103, 189), (197, 176, 213), (140, 86, 75), (196, 156, 148),  
             (227, 119, 194), (247, 182, 210), (127, 127, 127), (199, 199, 199),  
             (188, 189, 34), (219, 219, 141), (23, 190, 207), (158, 218, 229)]  
   
for i in range(len(tableau20)):  
    r, g, b = tableau20[i]  
    tableau20[i] = (r / 255., g / 255., b / 255.)  

data = csv2rec("data.csv", delimiter=';')

plt.figure(figsize=(10, 7.5))
#                                           OTP     cal
print data['ax'][20:].mean() # = 4164       0xf2d2  0xf28e
print data['ay'][20:].mean() # =  163       0x02e9  0x0246
print data['az'][20:].mean() # = -622       0x05a2  0x0810

for n, c in enumerate(data.dtype.names[0:3]):
    line = plt.plot(data[c], color=tableau20[n], label=c, linewidth=1.5)

plt.title('Aceleraciones')
plt.legend()
plt.xlim((0,1000))

#plt.savefig("figuras/accel.pdf")
plt.show(block=False)

plt.figure(figsize=(10, 7.5))

for n, c in enumerate(data.dtype.names[3:6]):
    line = plt.plot(data[c], color=tableau20[n], label=c, linewidth=1.5)

plt.title('Giroscopio')
plt.legend()
plt.xlim((0,1000))

#plt.savefig("figuras/gyros.pdf")
plt.show(block=False)

alpha = 0.02
tilt_r = atan2(data.ay, data.az)*180/pi
tilt_gtmp = (data.gx*65.5/(32.8*200.0)).cumsum()
tilt_g = tilt_gtmp + tilt_r[0]

tilt = zeros(tilt_r.shape)
tilt[0] = tilt_r[0]
for i in xrange(1,tilt_r.size):
    tilt[i] = (1-alpha)*(tilt[i-1] + data.gx[i]*65.5/(32.8*200.0)) + alpha*tilt_r[i]

fig = plt.figure(figsize=(10, 6))

plt.plot(tilt_r, color=tableau20[0], label='atan2(ay,az)', linewidth=2)
plt.plot(tilt_g, color=tableau20[1], label='int(gx dt)', linewidth=2)
plt.plot(tilt, color=tableau20[2], label='Filtro comp', linewidth=2)

plt.title(u'Inclinación')
plt.xlim((0,1000))

plt.legend()
plt.tight_layout()
plt.show(block=True)
#plt.savefig("figuras/tilt.pdf")