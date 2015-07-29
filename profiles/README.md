# profiles #

A profile is a yaml file describing the pin configuration of the
sensor board.  Mostly, I've been using teensy 2.0 and teensy 2.0++
boards, so the hardware support here is so far mostly for the
AT90USB1286 (teensy 2.0++) and ATmega32U4 (teensy 2.0).

I plan to add support for ATtiny88, as those are the actual sensors
I plan on building an deploying.

In the meantime, here is what the profile must look like:

~~~
mcu: at90usb1286                         # REQ: or atmega32u4
f_cpu: 8000000                           # REQ: or appropriate cpu clock speed
debug: true                              # REQ: or false, controls serial debug
rx_address: XXXXXXXXXX                   # REQ: 5 byte address for RX pipe 0
tx_address: XXXXXXXXXX                   # REQ: 5 byte address for receiving hub

uart:                                    # REQ if debug true
  type: soft                             # OPT: defaults to soft, hard not impl
  baud: 9600                             # REQ if uart
  port: D                                # REQ if uart
  pin: 7                                 # REQ if uart - tx pin for serial

switch:                                  # OPT: if sending switch state updates
  irq_pin: 7                             # REQ: what IRQ for external interrupt
  port: F                                # REQ: what port the switches are on
  switches:
    - { pin: 0 }                         # REQ: 1 or more switch pins on port
    - { pin: 1 }
    - { pin: 2 }
~~~

Right now, everyting is wired up by polling.  I expect to add sleep mode.
