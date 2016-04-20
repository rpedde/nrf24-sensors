/*
 * switch.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "hardware.h"
#include "switch.h"

static char *label = "sw : ";

void switch_init(void) {
    int x;

    /* Set directions on switch ports */
    for(x = 0; x < SWITCH_LENGTH; x++) {
        /* set to input */
        CLEARBIT(SWITCH_DDR, switch_records[x].pin);
        switch_records[x].state = -1;
        /* enable pullup */
        SETBIT(SWITCH_PORT, switch_records[x].pin);
    }

    /* enable pin change */
    PCICR = _BV(PCIE1);
    PCIFR = _BV(PCIF1);
    PCMSK1 = _BV(PCINT10) | _BV(PCINT11) | _BV(PCINT12) | _BV(PCINT13);

    sei();
}

void switch_sleep(void) {
}

void switch_wake(void) {
}

float switch_get(int which) {
    int val;

    val = SWITCH_PIN & _BV(switch_records[which].pin) ? 0 : 1;

    if(val == switch_records[which].state)
        return -1;

    switch_records[which].state = val;
    DPUTS(label); DPUTBYTEX(which); DPUTS("->"); DPUTBYTEX(val); DCR;
    return val;
}
