#include <avr/io.h>
#include <stdio.h>

#include <util/delay.h>
#include <avr/interrupt.h>

#include "soft_uart.h"
#include "spi.h"
#include "nrf24.h"
#include "util.h"

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
}

int main(int argc, char *argv[]) {
    char *spinner="/-\\|";

    CPU_PRESCALE(0x01);

    uart_init();

    DPRINTF("\x1b[2J\x1b[1;1H** Start **\n\r\n\r");

    nrf24_init();

    dump_status();

    DPRINTF("Turning on radio\n\r");
    nrf24_config_tx();

    DPRINTF("New status\n\r");
    dump_status();

    _delay_ms(100);

    while(1) {
        for(uint8_t x=0; x<4; x++) {
            uart_send_char(spinner[x]);
            uart_send_char('\r');
            nrf24_transmit((uint8_t *)"hello", 5);
            _delay_ms(1000);
        }
    }
}
