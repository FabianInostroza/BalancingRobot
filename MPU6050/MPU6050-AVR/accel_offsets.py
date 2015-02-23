#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
from time import sleep

def hex2int16(h):
    i = int(h, 16)
    return (i-(i&0x8000)*2)

def int2hex16(i):
    if (i < 0):
        n = (1 << 16) + i
    else:
        n = i
    return '%x'%n

#with serial.Serial('/dev/ttyUSB0', 115200) as ser:
with serial.Serial('/dev/rfcomm0', 115200) as ser:
    ser.flush()
    i = 0
    ax = 0
    ay = 0
    az = 0
    while i < 1000:
        try:
            line = ser.readline()
            line = line.strip('\x00')
            data = line.split(';')
            ax = ax*0.95 + 0.05*hex2int16(data[0])
            ay = ay*0.95 + 0.05*hex2int16(data[1])
            az = az*0.95 + 0.05*hex2int16(data[2])
            i += 1
        except:
            pass

print ax, ay, az

# promedios con offset = 0
# 31965.0370669 -5721.71090224 433.681089854 (sens = +/-2g)
# 7890.67742199 -1585.56594261 111.022959196 (sens = +/-8g)
# 3994.32466256 -716.033944432 49.9634944087 (sens = +/-16g)

# promedios sin modificar offsets (-3374, 745, 1442)
# 316.859510318 513.864198711 14130.347425 (sens = +/-2g)
# -18.6095962241 -29.2631997802 3532.80682933 (sens = +/-8g)
# -17.9247894462 -28.3212060937 3534.6535247 (sens = +/-8g)
# 39.631619598 62.0872113144 1759.82587712 (sens = +/-16g)
# 40.2412786093 62.5467850864 1762.66762608 (sens = +/-16g)
