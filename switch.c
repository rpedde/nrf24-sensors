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

    PCMSK0 = 0;
    PCMSK1 = 0;
    PCMSK2 = 0;
    PCMSK3 = 0;

    /* Set directions on switch ports */
    for(x = 0; x < SWITCH_LENGTH; x++) {
        /* set to input */
        *switch_records[x].DDR &= ~_BV(switch_records[x].pin);
        /* enable pullup */
        *switch_records[x].PORT |= _BV(switch_records[x].pin);
        /* set initial state */
        switch_records[x].state = -1;

        if(switch_records[x].pcint < 8) {
            PCMSK0 |= _BV(switch_records[x].pcint);
            PCICR |= _BV(PCIE0);
            PCIFR |= _BV(PCIF0);
        } else if(switch_records[x].pcint < 16) {
            PCMSK1 |= _BV(switch_records[x].pcint - 8);
            PCICR |= _BV(PCIE1);
            PCIFR |= _BV(PCIF1);
        } else if(switch_records[x].pcint < 24) {
            PCMSK2 |= _BV(switch_records[x].pcint - 16);
            PCICR |= _BV(PCIE2);
            PCIFR |= _BV(PCIF2);
        } else {
            PCMSK3 |= _BV(switch_records[x].pcint - 24);
            PCICR |= _BV(PCIE3);
            PCIFR |= _BV(PCIF3);
        }
    }

    /* enable pin change */
    /* PCICR = _BV(PCIE1); */
    /* PCIFR = _BV(PCIF1); */
    /* PCMSK1 = _BV(PCINT10) | _BV(PCINT11) | _BV(PCINT12) | _BV(PCINT13); */

    sei();
}

void switch_sleep(void) {
}

void switch_wake(void) {
}

float switch_get(int which) {
    int val;

    val = *switch_records[which].PIN & _BV(switch_records[which].pin) ? 0 : 1;

    if(val == switch_records[which].state)
        return -1;

    switch_records[which].state = val;
    DPUTS(label); DPUTBYTEX(which); DPUTS("->"); DPUTBYTEX(val); DCR;
    return val;
}
