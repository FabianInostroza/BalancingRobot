#!/usr/bin/env python2

from serial.tools.list_ports import comports
import serial
from time import sleep

ser = serial.Serial('/dev/ttyUSB0', 38400)

sleep(2)

for i in xrange(0, 256):
    m = "P:%X:\n"%i
    ser.write(m)
    sleep(0.2)
    print ser.read(ser.inWaiting())