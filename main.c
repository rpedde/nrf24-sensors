#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

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

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

void init_sensors(void) {
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

void sleep(void) {
#ifdef BATTERY_SENSOR
    battery_sleep();
#endif

#ifdef SWITCH_SENSOR
    switch_sleep();
#endif

#ifdef DHT_SENSOR
    dht_sleep();
#endif
}

void wake(void) {
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

    while(1) {
        _delay_ms(10000);

#ifdef BATTERY_SENSOR
        if(battery_get()) {
            packet.type = SENSOR_TYPE_VOLTAGE;
            packet.model = VOLT_MODEL_16B_2X33VREF;
            packet.type_instance = 0;
            packet.value.uint16_value = battery_read();
            nrf24_transmit((uint8_t *)&packet, sizeof(packet));
        }
#endif

#ifdef SWITCH_SENSOR
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

#ifdef DHT_SENSOR
        if(dht_read_data()) {
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

        _delay_ms(20000);

        /* while(!switch_irq); */
        /* switch_irq = 0; */
    }
}
