#!/bin/bash

export WIRINGPI_CODES=1

/usr/local/bin/gpio load i2c

/usr/local/bin/gpio export 17 in  #GPIO 17, RC5-IN,       pin 11
/usr/local/bin/gpio export 5 in   #GPIO 5,  Taster 1,     pin 28
/usr/local/bin/gpio export 6 in   #GPIO 6,  Taster 2,     pin 31
/usr/local/bin/gpio export 13 in  #GPIO 13, Taster 3,     pin 33
/usr/local/bin/gpio export 12 in  #GPIO 12, Taster 4,     pin 32

/usr/local/bin/gpio export 4 out  #GPIO 04, Input Select, pin 7
/usr/local/bin/gpio export 27 out #GPIO 27, RC5-Direct,   pin 13
/usr/local/bin/gpio export 26 out #GPIO 26, LED-Power,    pin 37
/usr/local/bin/gpio export 23 out #GPIO 23, Relais,       pin 16

export UPPLAY_LOGLEVEL=1
export UPPLAY_UPNPLOGFILENAME=/home/pi/RaspiDAC/libupnp.log
export UPPLAY_UPNPLOGLEVEL=1 

/home/pi/RaspiDAC/RaspiDAC -platform linuxfb:fb=/dev/fb1 &>/home/pi/RaspiDAC/RaspiDAC.log
