#!/usr/bin/env python2

import serial

class MPU6050(object):
    def __init__(self, port, dir=0x68):
        self._dir = dir
        self._port = port

    def write(self,reg, val):
        self._port.write("W:%X:%X:%X:"%(self._dir, reg, val))
        print self._port.readline()

    def read(self,reg):
        self._port.write("R:%X:%X:"%(self._dir, reg))
        res = self._port.readline()
        print res
        res = res.split(':')
        return int(res[3], 16)


ser = serial.Serial('/dev/ttyUSB0', 38400)

mpu = MPU6050(ser)

mpu.write(0x1a, 1) # accel bw=184Hz, gyro bw = 188Hz

mpu.write(0x19, 1) # sample rate divider = 2, fs = 1kHz/2 = 500Hz, T=2ms
mpu.write(0x1b, 0xe0 & 0x00)
mpu.write(0x1c, 0xf0 & 0x10)

mpu.read(0x0d)
mpu.read(0x0e)
mpu.read(0x0f)
mpu.read(0x10)

# pin INT active high, push-pull, 
# pulso 50us, cualquier lectura borra status
mpu.write(0x37, 0x80)

mpu.write(0x38, 0x01) # interrupcion en nuevos datos

axh = mpu.read(0x3b)
axl = mpu.read(0x3c)

ayh = mpu.read(0x3d)
ayl = mpu.read(0x3e)

azh = mpu.read(0x3f)
azl = mpu.read(0x40)

ax = (axh*256+axl)
ay = (ayh*256+ayl)
az = (azh*256+azl)

# if ax & 0x8000:
#     ax = -(((ax ^ 0xFFFF) + 1)&0xFFFF)/4096.0
# else:
#     ax = ax/4096.0
# ay = -(((ay ^ 0xFFFF) + 1)&0xFFFF)/4096.0
# az = -(((az ^ 0xFFFF) + 1)&0xFFFF)/4096.0

ax = (ax-(ax&0x8000)*2)/4096.0
ay = (ay-(ay&0x8000)*2)/4096.0
az = (az-(az&0x8000)*2)/4096.0

print "ax ", ax
print "ay ", ay
print "az ", az

print (ax**2+ay**2 +az**2)**0.5