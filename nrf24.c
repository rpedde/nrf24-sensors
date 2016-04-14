/*
 * Nrf24.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "hardware.h"
#include "util.h"
#include "spi.h"
#include "nrf24.h"
#include "soft_uart.h"

static char *label="nRF: ";

static void fix_addr(uint8_t *src, uint8_t *dst, uint8_t len) {
    uint8_t idx=0;

    for(idx = 0; idx < len; idx++) {
        dst[idx] = src[len-idx-1];
    }
}

void nrf24_init(void) {
    /* turn on SPI */
    DPUTS(label); DPUTS("init:0"); DCR;

    /* wait for powerup */
    _delay_ms(100);

    /* set the CE to output */
    SETBIT(NRF_CE_DDR, NRF_CE_PIN);

    spi_init(SPI_MODE_0, SPI_FOSC_16, SPI_MASTER);

    /* turn off CE */
    nrf24_enable(FALSE);

    _delay_ms(100);

    DPUTS(label); DPUTS("init:1"); DCR;
}

void nrf24_config_tx(uint8_t *tx_addr, uint8_t *rx_addr) {
    uint8_t inverted_addr[5];

    /* TX Mode, with CRC, no interrupts, 16 bit crc */
    nrf24_write_register(REG_CONFIG, ((1 << REG_CONFIG_MASK_RX_DR) |
                                      (1 << REG_CONFIG_MASK_TX_DR) |
                                      (1 << REG_CONFIG_MASK_MAX_RT) |
                                      (1 << REG_CONFIG_EN_CRC) |
                                      (1 << REG_CONFIG_EN_CRCO)));

    /* disable AA on all pipes */
    nrf24_write_register(REG_EN_AA, 0x00);

    /* Set address width to 3 (5 bytes) - match rpi */
    nrf24_write_register(REG_SETUP_AW, 0x03);

    /* Turn off auto retransmission */
    nrf24_write_register(REG_SETUP_RETR, 0x00);

    /* Set rf channel to match rpi */
    nrf24_write_register(REG_RF_CH, 0x4c);

    /* setup air data rate to 1Mbsp to match rpi */
    nrf24_write_register(REG_RF_SETUP, 0x07);

    /* set tx addr */
    fix_addr(tx_addr, inverted_addr, 5);
    nrf24_read_write_vector_register(REG_WRITE,
                                     REG_TX_ADDR,
                                     inverted_addr,
                                     5);

    /* set rx addr 0 */
    fix_addr(rx_addr, inverted_addr, 5);
    nrf24_read_write_vector_register(REG_WRITE,
                                     REG_RX_ADDR_P0,
                                     inverted_addr,
                                     5);

    /* enable read pipe 0 */
    nrf24_write_register(REG_EN_RXADDR, 1 << REG_RXADDR_ERX_P0);

    /* set rx size */
    nrf24_write_register(REG_RX_PW_P0, 11);

    nrf24_power_up(TRUE);
}

void nrf24_config_rx(uint8_t *rx_addr) {
    /* TX Mode, with CRC, no interrupts, 16 bit crc */
    nrf24_write_register(REG_CONFIG, ((1 << REG_CONFIG_MASK_RX_DR) |
                                      (1 << REG_CONFIG_MASK_TX_DR) |
                                      (1 << REG_CONFIG_MASK_MAX_RT) |
                                      (1 << REG_CONFIG_EN_CRC) |
                                      (1 << REG_CONFIG_EN_CRCO) |
                                      (1 << REG_CONFIG_EN_PRIM_RX)));

    /* disable AA on all pipes */
    nrf24_write_register(REG_EN_AA, 0x00);

    /* Set address width to 3 (5 bytes) - match rpi */
    nrf24_write_register(REG_SETUP_AW, 0x03);

    /* Turn off auto retransmission */
    nrf24_write_register(REG_SETUP_RETR, 0x00);

    /* Set rf channel to match rpi */
    nrf24_write_register(REG_RF_CH, 0x4c);

    /* setup air data rate to 1Mbsp to match rpi */
    nrf24_write_register(REG_RF_SETUP, 0x07);

    /* set rx addr */
    nrf24_read_write_vector_register(REG_WRITE,
                                     REG_RX_ADDR_P0,
                                     rx_addr,
                                     5);

    /* enable read pipe 0 */
    nrf24_write_register(REG_EN_RXADDR, 1 << REG_RXADDR_ERX_P0);

    /* set rx size */
    nrf24_write_register(REG_RX_PW_P0, 5);

    nrf24_power_up(TRUE);
    _delay_ms(100);
}

int nrf24_receive(uint8_t *buf, uint8_t len, uint8_t pipe) {
    uint8_t actual_len;

    nrf24_reset_irq();
    nrf24_write_register(REG_FLUSH_RX, 0);

    nrf24_enable(TRUE);
    _delay_ms(1000);
    nrf24_enable(FALSE);

    uint8_t status = nrf24_read_register(REG_STATUS);
    if(ISCLEAR(status, REG_STATUS_RX_DR))
        return FALSE;

    /* find the length of data in the buffer */
    actual_len = nrf24_read_register(REG_RX_PW_P0 + pipe);

    nrf24_read_write_vector_register(REG_READ,
                                     REG_READ_RX,
                                     buf,
                                     actual_len);
    nrf24_write_register(REG_FLUSH_RX, 0);
    return TRUE;
}

void nrf24_reset_irq(void) {
    nrf24_write_register(REG_STATUS, 0x70);
}

void nrf24_transmit(uint8_t *buf, uint8_t len) {
    DPUTS(label); DPUTS("tx "); DPUTBYTEX(len); DCR;
    nrf24_write_register(REG_FLUSH_TX, 0);
    nrf24_read_write_vector_register(REG_WRITE,
                                     REG_WRITE_TX,
                                     buf,
                                     len);
    _delay_ms(10);
    nrf24_enable(TRUE);
    _delay_us(20);
    nrf24_enable(FALSE);
    _delay_ms(10);
    nrf24_reset_irq();
}

void nrf24_power_up(int powerup) {
    uint8_t val = nrf24_read_register(REG_CONFIG);
    if(powerup) {
        SETBIT(val, REG_CONFIG_EN_PWR_UP);
    } else {
        CLEARBIT(val, REG_CONFIG_EN_PWR_UP);
    }
    nrf24_write_register(REG_CONFIG, val);
}

void nrf24_enable(int enable) {
    if(enable) {
        SETBIT(NRF_CE_PORT, NRF_CE_PIN);
    } else {
        CLEARBIT(NRF_CE_PORT, NRF_CE_PIN);
    }
    _delay_us(10);
}

void nrf24_read_write_vector_register(uint8_t direction, /* REG_READ/WRITE */
                                      uint8_t reg,       /* register */
                                      uint8_t *buffer,   /* buffer */
                                      uint8_t len) {     /* buffer size */

    CLEARBIT(SPI_PORT, SPI_SS_PIN);

    if (reg < REG_WRITE)
        reg |= direction;

    if (reg == REG_FLUSH_RX || reg == REG_FLUSH_TX)
        len = 0;

    spi_send(reg);

    _delay_us(10);
    for(uint8_t idx=0; idx<len; idx++) {
        if (direction == REG_READ) {
            buffer[idx] = spi_send(REG_NOP);
        } else {
            spi_send(buffer[idx]);
        }
        _delay_us(10);
    }

    SETBIT(SPI_PORT, SPI_SS_PIN);
    _delay_us(10);
}

uint8_t nrf24_read_register(uint8_t reg) {
    uint8_t res;

    nrf24_read_write_vector_register(REG_READ, reg, &res, 1);
    return res;
}

void nrf24_write_register(uint8_t reg, uint8_t value) {
    if(reg == REG_CONFIG) {
        DPUTS(label); DPUTBYTEX(value); DPUTS("->CFG"); DCR;
    }
    nrf24_read_write_vector_register(REG_WRITE, reg, &value, 1);
}
