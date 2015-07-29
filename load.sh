#!/bin/bash

if [ "$1" == "teensy" ]; then
    MCU=atmega32u4
    echo "Loading Teensy 2.0"
elif [ "$1" == "teensy++" ]; then
    MCU=at90usb1286
    echo "Loading Teensy 2.0++"
fi

if [ "${MCU}" == "" ]; then
    echo "unrecognized board"
    exit 1
fi


teensy_loader_cli -w --mcu=${MCU} main.hex
