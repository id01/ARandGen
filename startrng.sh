#!/bin/sh

MYDEVICE="/dev/ttyACM0"

stty -F "$MYDEVICE" 9600 raw -clocal -echo icrnl
rngd -r "$MYDEVICE"
