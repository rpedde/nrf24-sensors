# nrf24-sensors #

A nrf24 library for AVR, not arduino, along with a build system
for building low-powered sensors using nrf24.  I'm currently testing this
on teensy 2.0 (ATmega32U4) and teensy++ 2.0 (AT90USB1286).
In the future, I'll probably port this to ATmega88/168 (since I
have a bunch laying around) and ATtiny88 for super-cheap sensors.

This isn't very generalized -- it's mostly for my needs and
on the boards I have.  Maybe it works for you, maybe it doesn't.
PRs welcome.

# TODO #

## General ##
- [ ] Low power/sleep mode rather than polling

## TX sensors ##
- [ ] TMP36 temperature sensor
- [X] DHT22 temperature/humidity sensor
- [X] DHT11 temperature/humidity sensor
- [ ] DS18B20 temperature sensor
- [X] Switch monitoring

## RX sensors ##
- [ ] Receive for setting port outputs (rw switch)
- [ ] Receive for controlling RGB LED

# Uses #

This goes hand-in-hand with https://github.com/rpedde/nrf24-mqtt, a
mqtt gateway for nrf24 sensors (for raspberry pi)
