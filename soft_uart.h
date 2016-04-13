/*
 * uart.h Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

/*
 * soft uart implementation
 */

#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

#define UART_SOFT_DELAY_US (int) (1000000.0/((float) UART_SOFT_BAUD) + 0.5)

extern void uart_init(void);
extern void uart_send_char(uint8_t byte);
extern void uart_send_string(char *str);
extern void uart_print_hex_byte(uint8_t byte);
extern void uart_print_hex_word(uint16_t word);
extern void uart_print_int(uint16_t word);
extern void uart_printf(char *fmt, ...);

#endif /* __UART_H__ */
