/*
 * spi.h Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

#if defined(__AVR_AT90USB1286__) || defined(__AVR_ATmega32U4__)
# define SPI_DDR      DDRB
# define SPI_PORT     PORTB
# define SPI_SS_PIN   0
# define SPI_SCK_PIN  1
# define SPI_MOSI_PIN 2
# define SPI_MISO_PIN 3
#else
# error Unhandled CPU in spi.h
#endif

#define SPI_MODE_0   0
#define SPI_MODE_1   1
#define SPI_MODE_2   2
#define SPI_MODE_3   3

#define SPI_FOSC_4   0
#define SPI_FOSC_16  1
#define SPI_FOSC_64  2
#define SPI_FOSC_128 3

#define SPI_MASTER   1
#define SPI_SLAVE    0

extern void spi_init(uint8_t mode, uint8_t freq, uint8_t master);
extern uint8_t spi_send(uint8_t byte);

#endif /* __SPI_H__ */
