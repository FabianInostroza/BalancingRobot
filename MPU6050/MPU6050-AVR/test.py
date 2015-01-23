#!/usr/bin/env python2

import serial
from time import sleep

class MPU6050(object):
    def __init__(self, port, dir=0x68):
        self._dir = dir
        self._port = port

    def write(self,reg, val):
        self._port.write("W:%X:%X:%X:"%(self._dir, reg, val))
        #print self._port.readline()
        self._port.readline()

    def read(self,reg):
        self._port.write("R:%X:%X:"%(self._dir, reg))
        res = self._port.readline()
        #print res
        res = res.split(':')
        return int(res[3], 16)
        
    def readAccel(self):
        axh = self.read(0x3b)
        axl = self.read(0x3c)

        ayh = self.read(0x3d)
        ayl = self.read(0x3e)

        azh = self.read(0x3f)
        azl = self.read(0x40)

        return (axh*256+axl, ayh*256+ayl,azh*256+azl)
        
    def readGyro(self):
        gxh = self.read(0x43)
        gxl = self.read(0x44)

        gyh = self.read(0x45)
        gyl = self.read(0x46)

        gzh = self.read(0x47)
        gzl = self.read(0x48)

        return (gxh*256+gxl, gyh*256+gyl, gzh*256+gzl)
        
    def readTestRegs(self):
        tmp = self.read(0x0d)
        xgt = tmp & 0x1f
        xat = (tmp & 0xc0) >> 3
        tmp = self.read(0x0e)
        ygt = tmp & 0x1f
        yat = (tmp & 0xc0) >> 3
        tmp = self.read(0x0f)
        zgt = tmp & 0x1f
        zat = (tmp & 0xc0) >> 3
        tmp = self.read(0x10)
        zat = zat | (tmp & 0x3)
        yat = yat | ((tmp >> 2) & 0x3)
        xat = xat | ((tmp >> 4) & 0x3)
        return (xat, yat, zat, xgt, ygt, zgt)


ser = serial.Serial('/dev/ttyUSB0', 115200)

mpu = MPU6050(ser)

mpu.write(0x1a, 1) # accel bw=184Hz, gyro bw = 188Hz

mpu.write(0x19, 1) # sample rate divider = 2, fs = 1kHz/2 = 500Hz, T=2ms
mpu.write(0x1b, 0xe0)
mpu.write(0x1c, 0xf0)

sleep(0.2)

tests = mpu.readTestRegs()

gyro_t = mpu.readGyro()
accel_t = mpu.readAccel()

FTgx = 25*131*1.046**(tests[3]) if tests[3] else 0
FTgy = -25*131*1.046**(tests[4]) if tests[4] else 0
FTgz = 25*131*1.046**(tests[5]) if tests[5] else 0

FTax = 4096*0.34*(0.92/0.34)**((tests[0]-1)/(2**5-2)) if tests[0] else 0
FTay = 4096*0.34*(0.92/0.34)**((tests[1]-1)/(2**5-2)) if tests[1] else 0
FTaz = 4096*0.34*(0.92/0.34)**((tests[2]-1)/(2**5-2)) if tests[2] else 0

print "FTg: ", (FTgx, FTgy, FTgz)
print "FTa: ", (FTax, FTay, FTaz)

mpu.write(0x1b, 0xe0 & 0x00)
mpu.write(0x1c, 0xf0 & 0x80)

sleep(0.2)

gyro = mpu.readGyro()
accel = mpu.readAccel()

STRgx = gyro_t[0] - gyro[0]
STRgy = gyro_t[1] - gyro[1]
STRgz = gyro_t[2] - gyro[2]

STRax = accel_t[0] - accel[0]
STRay = accel_t[1] - accel[1]
STRaz = accel_t[2] - accel[2]

print "Change from FT gx: ", (STRgx-FTgx)/FTgx
print "Change from FT gy: ", (STRgy-FTgy)/FTgy
print "Change from FT gz: ", (STRgz-FTgz)/FTgz

print "Change from FT ax: ", (STRax-FTax)/FTax
print "Change from FT ay: ", (STRay-FTay)/FTay
print "Change from FT az: ", (STRaz-FTaz)/FTaz

#mpu.read(0x0d)
#mpu.read(0x0e)
#mpu.read(0x0f)
#mpu.read(0x10)

# pin INT active high, push-pull, 
# pulso 50us, cualquier lectura borra status
#mpu.write(0x37, 0x80)

#mpu.write(0x38, 0x01) # interrupcion en nuevos datos

# if ax & 0x8000:
#     ax = -(((ax ^ 0xFFFF) + 1)&0xFFFF)/4096.0
# else:
#     ax = ax/4096.0
# ay = -(((ay ^ 0xFFFF) + 1)&0xFFFF)/4096.0
# az = -(((az ^ 0xFFFF) + 1)&0xFFFF)/4096.0

#ax = (ax-(ax&0x8000)*2)/4096.0
#ay = (ay-(ay&0x8000)*2)/4096.0
#az = (az-(az&0x8000)*2)/4096.0

#gx = (gx-(gx&0x8000)*2)/131.0
#gy = (gy-(gy&0x8000)*2)/131.0
#gz = (gz-(gz&0x8000)*2)/131.0

#print "ax ", ax
#print "ay ", ay
#print "az ", az
#print "gx ", gx
#print "gy ", gy
#print "gz ", gz

#print (ax**2+ay**2 +az**2)**0.5
