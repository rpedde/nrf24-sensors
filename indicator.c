/*
 * indicator.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#include "util.h"
#include "hardware.h"

void indicator_init(void) {
    /* set the indicator pin to output */
    SETBIT(INDICATOR_DDR, INDICATOR_PIN);
    SETBIT(INDICATOR_PORT, INDICATOR_PIN);
}

void indicator_sleep(void) {
    /* turn off the indicator pin */
    CLEARBIT(INDICATOR_PORT, INDICATOR_PIN);
}

void indicator_wake(void) {
    /* turn on the indictator pin */
    SETBIT(INDICATOR_PORT, INDICATOR_PIN);
#if INDICATOR_DELAY > 0
    _delay_ms(INDICATOR_DELAY);
#endif

}
