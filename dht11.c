/*
 * dht11.h Copyright (C) 2016 Ron Pedde <ron@pedde.com>
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

#include "dht11.h"

static uint8_t dht11_data[5];

void dht11_init(void) {
    DPRINTF("Initializing DHT11\n\r");
    // input
    CLEARBIT(DHT11_DDR, DHT11_PIN);
    _delay_ms(2000);
}

void dht11_sleep(void) {
}

void dht11_wake(void) {
}

static int dht11_wait_for_value(int val, int timeout) {
    int res;

    TCNT0 = 0;

    while(TCNT0 < timeout) {
        res = (DHT11_INP) & _BV(DHT11_PIN);
        if ((res == 0) && (val == 0))
            return TCNT0;

        if (res && val)
            return TCNT0;

        /* if((!res && !val) || (res && val)) */
        /*     return TCNT0; */
    }

    return TCNT0;
}


void dht11_read_data(void) {
    int res;

    SETBIT(DHT11_DDR, DHT11_PIN);       /* set to output */
    CLEARBIT(DHT11_OUTP, DHT11_PIN);    /* output low */

    /* This might need to be adjusted for dht11 and 22
     * Longer delay (50ms) seems to work fine on 11, but not 22. */

    _delay_ms(20);                      /* min 18ms */
    SETBIT(DHT11_OUTP, DHT11_PIN);      /* and high */

    CLEARBIT(DHT11_DDR, DHT11_PIN);     /* set for input */

    /* set up prescalar for timer0 */
    DHT11_PRESCALE_REG |= \
        (DHT11_CS00 << CS00) | \
        (DHT11_CS01 << CS01) | \
        (DHT11_CS02 << CS02);

    if((res = dht11_wait_for_value(0, DHT11_FORTY)) > DHT11_FORTY) {
        DPRINTF("DHT11: Init timeout: %d, %02X\n\r", res, DHT11_INP);
        return;
    }

    if((res = dht11_wait_for_value(1, DHT11_HUNDRED)) > DHT11_HUNDRED) {
        DPRINTF("DHT11: High timeout: %d, %02X\n\r", res, DHT11_INP);
        return;
    }

    if(dht11_wait_for_value(0, DHT11_HUNDRED) > DHT11_HUNDRED) {
        DPRINTF("DHT11: Low timeout\n\r");
        return;
    }

    for(int i=0; i<5; i++) {
        for(int j=0; j<8; j++) {
            if(dht11_wait_for_value(1, DHT11_EIGHTY) > DHT11_EIGHTY) {
                DPRINTF("DHT11: Timeout\n\r");
                return;
            }

            if(dht11_wait_for_value(0, DHT11_EIGHTY) > DHT11_FORTY) {
                SETBIT(dht11_data[i], 7-j);
            } else {
                CLEARBIT(dht11_data[i], 7-j);
            }
        }
    }

    /* should check the checksum */
    uint8_t data = dht11_data[0] + dht11_data[1] + \
        dht11_data[2] + dht11_data[3];

    if (data != dht11_data[4]) {
        DPRINTF("DHT11: Checksum error: %02X %02X %02X %02X %02X\n\r",
            dht11_data[0], dht11_data[1], dht11_data[2],
            dht11_data[3], dht11_data[4]);
    }

    for(int i=0; i<5; i++) {
        DPRINTF("%02X ", dht11_data[i]);
    }
    DPRINTF("\n\r");

    /* DHT11 format */
    DPRINTF("Humidity: %d.%d percent\n\r", dht11_data[0], dht11_data[1]);
    DPRINTF("Temp: %d.%d degrees C\n\r", dht11_data[2], dht11_data[3]);

    uint16_t h, t;
    h = dht11_data[0] << 8 | dht11_data[1];
    t = dht11_data[2] << 8 | dht11_data[3];

    /* DHT22 (tenths) */
    DPRINTF("Hum: %d\n\r", h);
    DPRINTF("Temp: %d\n\r", t);
}
