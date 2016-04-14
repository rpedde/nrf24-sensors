/*
 * uart.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#include <stdio.h>
#include <inttypes.h>
#include <stdarg.h>

#include <avr/io.h>
#include <util/delay.h>

#include "util.h"
#include "hardware.h"
#include "soft_uart.h"

void uart_init(void) {
    SETBIT(UART_SOFT_DDR, UART_SOFT_PIN);     /* out */
    SETBIT(UART_SOFT_PORT, UART_SOFT_PIN);    /* high */
}

void uart_send_char(uint8_t byte) {
    uint8_t rack;

    CLEARBIT(UART_SOFT_PORT, UART_SOFT_PIN);  /* low */

    _delay_us(UART_SOFT_DELAY_US);

    for(rack=0; rack<8; rack++) {
        if(byte & 1)
            SETBIT(UART_SOFT_PORT, UART_SOFT_PIN);
        else
            CLEARBIT(UART_SOFT_PORT, UART_SOFT_PIN);
        byte >>= 1;
        _delay_us(UART_SOFT_DELAY_US);
    }
    SETBIT(UART_SOFT_PORT, UART_SOFT_PIN);
    _delay_us(UART_SOFT_DELAY_US);
}

void uart_send_string(char *str) {
    while(*str)
        uart_send_char((uint8_t) *str++);
}

uint8_t _hex_nibble(uint8_t nibble) {
    if(nibble < 10)
        return 0x30 + nibble;
    return 0x41 + (nibble - 10);
}

void uart_print_hex_byte(uint8_t byte) {
    uart_send_char(_hex_nibble(byte >> 4));
    uart_send_char(_hex_nibble(byte & 0x0F));
}

void uart_print_hex_word(uint16_t word) {
    uart_print_hex_byte((uint8_t)(word & 0x00FF));
    uart_print_hex_byte((uint8_t)(word >> 8));
}

