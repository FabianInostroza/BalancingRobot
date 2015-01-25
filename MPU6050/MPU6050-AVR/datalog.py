#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial

def hex2int16(h):
    i = int(h, 16)
    return (i-(i&0x8000)*2)

sens = [16384.0]*3 + [65.5]*3

with serial.Serial('/dev/ttyUSB0', 115200) as ser:
    with open('data.csv','w') as f:
        ser.flush()
        while True:
            line = ser.readline()
            line = line.strip('\x00')
            data = line.split(';')
            if len(data) >= 6:
                for i in xrange(0, 5):
                    val = hex2int16(data[i])/sens[i]
                    f.write(str(val))
                    f.write(';')

                val = hex2int16(data[5])/sens[5]
                f.write(str(val))
                f.write('\r\n')
