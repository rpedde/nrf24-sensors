/*
 * spi.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#include "util.h"
#include "spi.h"

void spi_init(uint8_t mode, uint8_t freq, uint8_t master) {
    uint8_t spcr_out;

    SETBIT(SPI_DDR, SPI_MOSI_PIN);
    SETBIT(SPI_DDR, SPI_SCK_PIN);
    SETBIT(SPI_DDR, SPI_SS_PIN);
    CLEARBIT(SPI_DDR, SPI_MISO_PIN);

    spcr_out = 0;

    switch(mode) {
    case 0: /* CPOL=0, CPHA=0: leading sample rising, training setup falling */
        break;
    case 1: /* CPOL=0, CPHA=1: leading setup rising, trailing sample falling*/
        spcr_out |= (1 << CPHA);
        break;
    case 2: /* CPOL=1, CPHA=0: leading sample falling, trailing setup rising */
        spcr_out |= (1 << CPOL);
        break;
    case 3: /* CPOL=1, CPHA=1: leading setup falling, trailing sample rising */
        spcr_out |= (1 << CPHA);
        spcr_out |= (1 << CPOL);
        break;
    }

    /* set master or slave */
    if(master)
        spcr_out |= (1 << MSTR);

    /* set frequency divider */
    if(ISSET(freq, 1))
        SETBIT(spcr_out, SPR1);
    if(ISSET(freq, 0))
        SETBIT(spcr_out, SPR0);

    /* enable SPI */
    spcr_out |= (1 << SPE);

    SPCR = spcr_out;
    SETBIT(SPI_PORT, SPI_SS_PIN);
}

uint8_t spi_send(uint8_t byte) {
    SPDR = byte;

    while(!(SPSR & (1 << SPIF)));
    return SPDR;
}
