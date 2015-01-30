#!/bin/bash

avrdude -v -patmega1284p -c usbasp -e -U lock:w:0x3F:m "-Uflash:w:/media/Datos/Electronica/BalancingRobot/Robot-AVR/bin/Release/Robot-AVR.hex:i" -U lfuse:w:0xF7:m -U hfuse:w:0xDF:m -U efuse:w:0xFD:m -U lock:w:0x2F:m -B 1
