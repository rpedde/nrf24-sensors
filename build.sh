#!/bin/bash

set -eux

for mcu in at90usb1286 atmega32u4; do
    make clean
    if [ ${mcu} == "at90usb1286" ]; then
        MODE="TXMODE"
    else
        MODE="RXMODE"
    fi

    make MCU=${mcu} OPMODE=${MODE}
    mv main.hex main-${mcu}.hex
done
