/*
 * nrf24.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

#define SETBIT(byte, bit) (byte) |= (1 << (bit))
#define CLEARBIT(byte, bit) (byte) &= (~(1 << (bit)))
#define ISSET(byte, bit) (byte) & (1 << (bit))
#define ISCLEAR(byte, bit) !(ISSET((byte), (bit)))

#define FALSE 0
#define TRUE  1

#ifdef DEBUG
extern void uart_printf(char *fmt, ...);
extern void uart_send_string(char *);
extern void uart_print_hex_byte(uint8_t);
extern void uart_print_hex_word(uint16_t);

# define DPRINTF(fmt, args...) uart_printf(fmt, ##args)
# define DPUTS(str) uart_send_string((str))
# define DPUTBYTEX(byte) uart_print_hex_byte((byte))
# define DPUTWORDX(word) uart_print_hex_word((word))
# define DCR uart_send_string("\n\r") 
#else
# define DPRINTF(fmt, args...)
# define DPUTS(str)
# define DPUTBYTEX(byte)
# define DPUTWORDX(word)
# define DCR
#endif



#endif /* __UTIL_H__ */
