#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include "soft_uart.h"
#include "spi.h"
#include "nrf24.h"
#include "util.h"
#include "hardware.h"
#include "sensor.h"

#ifdef BATTERY_SENSOR
#include "battery.h"
#endif

#ifdef SWITCH_SENSOR
#include "switch.h"
#endif

#ifdef DHT_SENSOR
#include "dht.h"
#endif

#ifdef SLEEP_INDICATOR
#include "indicator.h"
#endif

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define DEBOUNCE_TIME 10  // time to stable in ms

volatile int irq_wdt = 1;
volatile int irq_switch = 1;

ISR(WDT_vect) {
    irq_wdt = 1;
}

void init_sensors(void) {
#ifdef SLEEP_INDICATOR
    indicator_init();
#endif

#ifdef BATTERY_SENSOR
    battery_init();
#endif

#ifdef SWITCH_SENSOR
    switch_init();
#endif

#ifdef DHT_SENSOR
    dht_init();
#endif
}

void wake(void) {
#ifdef SLEEP_INDICATOR
    indicator_wake();
#endif

#ifdef BATTERY_SENSOR
    battery_wake();
#endif

#ifdef SWITCH_SENSOR
    switch_wake();
#endif

#ifdef DHT_SENSOR
    dht_wake();
#endif

}

void sleep(void) {
    int count = 0;

#ifdef BATTERY_SENSOR
    battery_sleep();
#endif

#ifdef SWITCH_SENSOR
    switch_sleep();
#endif

#ifdef DHT_SENSOR
    dht_sleep();
#endif

#ifdef SLEEP_INDICATOR
    indicator_sleep();
#endif

    sei();

    while(count < 4) {  /* 24 seconds */
        irq_wdt = 0;
        irq_switch = 0;

        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_mode();

        /* we break early on switch press */
        if(irq_switch)
            break;

        count++;
    }

    sleep_disable();
    wake();
}


void init_wdt(void) {
    SETBIT(MCUSR, WDRF);
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = _BV(WDP0) | _BV(WDP3);
    SETBIT(WDTCSR, WDIE);
}


int main(int argc, char *argv[]) {
    uint8_t osccal = 0;
    sensor_struct_t packet;

#ifdef SOFT_PRESCALAR
    CPU_PRESCALE(SOFT_PRESCALAR);
#endif

    uart_init();

#ifdef OSCCAL_EEPROM_ADDR
    osccal = eeprom_read_byte((uint8_t*)OSCCAL_EEPROM_ADDR);
    OSCCAL = osccal;
#endif

    DPUTS("\x1b[2J\x1b[1;1HStartup\n\r");

    nrf24_init();
    nrf24_config_tx(tx_address, rx_address);

    /* dump_status(); */

    _delay_ms(100);

    memcpy(packet.addr, rx_address, sizeof(packet.addr));

    /* init hardware sensors */
    init_sensors();

    init_wdt();

    while(1) {

        if(irq_wdt) {
#ifdef BATTERY_SENSOR
            if(battery_get()) {
                packet.type = SENSOR_TYPE_VOLTAGE;
                packet.model = VOLT_MODEL_16B_2X33VREF;
                packet.type_instance = 0;
                packet.value.uint16_value = battery_read();
                nrf24_transmit((uint8_t *)&packet, sizeof(packet));
            }
#endif

#ifdef DHT_SENSOR
            if(dht_get()) {
                packet.type = SENSOR_TYPE_HUMIDITY;
                packet.model = DHT_SENSOR_MODEL;
                packet.type_instance = 0;
                packet.value.uint16_value = dht_get_rh();
                nrf24_transmit((uint8_t *)&packet, sizeof(packet));

                packet.type = SENSOR_TYPE_TEMP;
                packet.value.uint16_value = dht_get_temp();
                nrf24_transmit((uint8_t *)&packet, sizeof(packet));
            }
#endif
        }

        if(irq_switch) {
#ifdef SWITCH_SENSOR
            _delay_ms(DEBOUNCE_TIME);

            for(int x = 0; x < SWITCH_LENGTH; x++) {
                int val = switch_get(x);
                if (val != -1) {
                    packet.type = SENSOR_TYPE_RO_SWITCH;
                    packet.model = SENSOR_MODEL_NONE;
                    packet.type_instance = x;
                    packet.value.uint8_value = val;
                    nrf24_transmit((uint8_t *)&packet, sizeof(packet));
                }
            }
#endif
        }

        sleep();
    }
}
