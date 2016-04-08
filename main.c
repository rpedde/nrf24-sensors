#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include <util/delay.h>
#include <avr/interrupt.h>

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

#ifdef DHT11_SENSOR
#include "dht11.h"
#endif

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

void dump_status(void) {
    uint8_t addrbuffer[5];

    DPRINTF(" CONFIG:      %02x\n\r", nrf24_read_register(REG_CONFIG));
    DPRINTF(" EN_AA:       %02x\n\r", nrf24_read_register(REG_EN_AA));
    DPRINTF(" EN_RXADDR:   %02x\n\r", nrf24_read_register(REG_EN_RXADDR));
    DPRINTF(" SETUP_AW:    %02x\n\r", nrf24_read_register(REG_SETUP_AW));
    DPRINTF(" SETUP_RETR:  %02x\n\r", nrf24_read_register(REG_SETUP_RETR));
    DPRINTF(" RF_CH:       %02x\n\r", nrf24_read_register(REG_RF_CH));
    DPRINTF(" RF_SETUP:    %02x\n\r", nrf24_read_register(REG_RF_SETUP));

    nrf24_read_write_vector_register(REG_READ,
                                     REG_TX_ADDR,
                                     addrbuffer,
                                     5);
    DPRINTF(" TX_ADDR:     %02x%02x%02x%02x%02x\n\r",
            addrbuffer[0], addrbuffer[1], addrbuffer[2],
            addrbuffer[3], addrbuffer[4]);

    nrf24_read_write_vector_register(REG_READ,
                                     REG_RX_ADDR_P0,
                                     addrbuffer,
                                     5);

    DPRINTF(" RX_ADDR:     %02x%02x%02x%02x%02x\n\r",
            addrbuffer[0], addrbuffer[1], addrbuffer[2],
            addrbuffer[3], addrbuffer[4]);
}

void init_sensors(void) {
#ifdef BATTERY_SENSOR
    battery_init();
#endif

#ifdef SWITCH_SENSOR
    switch_init();
#endif

#ifdef DHT11_SENSOR
    dht11_init();
#endif
}

void sleep(void) {
#ifdef BATTERY_SENSOR
    battery_sleep();
#endif

#ifdef SWITCH_SENSOR
    switch_sleep();
#endif

#ifdef DHT11_SENSOR
    dht11_sleep();
#endif
}

void wake(void) {
#ifdef BATTERY_SENSOR
    battery_wake();
#endif

#ifdef SWITCH_SENSOR
    switch_wake();
#endif

#ifdef DHT11_SENSOR
    dht11_wake();
#endif
}

int main(int argc, char *argv[]) {
    sensor_struct_t packet;

    CPU_PRESCALE(0x01);

    uart_init();

    DPRINTF("\x1b[2J\x1b[1;1H** Start **\n\r\n\r");
    DPRINTF("sizeof packet: %d\n\r", sizeof(packet));

    nrf24_init();
    nrf24_config_tx(tx_address, rx_address);

    dump_status();

    _delay_ms(100);

    memcpy(packet.addr, rx_address, sizeof(packet.addr));

    /* init hardware sensors */
    init_sensors();

    while(1) {
        _delay_ms(2000);

#ifdef BATTERY_SENSOR
        battery_get();
#endif

#ifdef SWITCH_SENSOR
        for(int x = 0; x < SWITCH_LENGTH; x++) {
            int val = switch_get(x);
            if (val != -1) {
                DPRINTF("Switch %d: new state: %d\n\r", x, val);
                packet.type = SENSOR_TYPE_RO_SWITCH;
                packet.type_instance = x;
                packet.value.uint8_value = val;
                nrf24_transmit((uint8_t *)&packet, sizeof(packet));
            }
        }
#endif

#ifdef DHT11_SENSOR
        dht11_read_data();
#endif

        _delay_ms(8000);

        /* while(!switch_irq); */
        /* switch_irq = 0; */
    }
}
