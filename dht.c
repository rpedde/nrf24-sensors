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
#include "sensor.h"

#include "dht.h"

static uint8_t dht_data[5];

void dht_init(void) {
    DPRINTF("Initializing DHT\n\r");
    // input
    CLEARBIT(DHT_DDR, DHT_PIN);
    _delay_ms(2000);
}

void dht_sleep(void) {
}

void dht_wake(void) {
}

static int dht_wait_for_value(int val, int timeout) {
    int res;

    TCNT0 = 0;

    while(TCNT0 < timeout) {
        res = (DHT_INP) & _BV(DHT_PIN);
        if ((res == 0) && (val == 0))
            return TCNT0;

        if (res && val)
            return TCNT0;

        /* if((!res && !val) || (res && val)) */
        /*     return TCNT0; */
    }

    return TCNT0;
}


int dht_read_data(void) {
    int res;

    SETBIT(DHT_DDR, DHT_PIN);       /* set to output */
    CLEARBIT(DHT_OUTP, DHT_PIN);    /* output low */

    /* This might need to be adjusted for dht11 and 22
     * Longer delay (50ms) seems to work fine on 11, but not 22. */

    _delay_ms(20);                  /* min 18ms */
    SETBIT(DHT_OUTP, DHT_PIN);      /* and high */

    CLEARBIT(DHT_DDR, DHT_PIN);     /* set for input */

    /* set up prescalar for timer0 */
    DHT_PRESCALE_REG |= \
        (DHT_CS00 << CS00) | \
        (DHT_CS01 << CS01) | \
        (DHT_CS02 << CS02);

    if((res = dht_wait_for_value(0, DHT_FORTY)) > DHT_FORTY) {
        DPRINTF("DHT: Init timeout: %d, %02X\n\r", res, DHT_INP);
        return FALSE;
    }

    if((res = dht_wait_for_value(1, DHT_HUNDRED)) > DHT_HUNDRED) {
        DPRINTF("DHT: High timeout: %d, %02X\n\r", res, DHT_INP);
        return FALSE;
    }

    if(dht_wait_for_value(0, DHT_HUNDRED) > DHT_HUNDRED) {
        DPRINTF("DHT: Low timeout\n\r");
        return FALSE;
    }

    for(int i=0; i<5; i++) {
        for(int j=0; j<8; j++) {
            if(dht_wait_for_value(1, DHT_EIGHTY) > DHT_EIGHTY) {
                DPRINTF("DHT: Timeout\n\r");
                return FALSE;
            }

            if(dht_wait_for_value(0, DHT_EIGHTY) > DHT_FORTY) {
                SETBIT(dht_data[i], 7-j);
            } else {
                CLEARBIT(dht_data[i], 7-j);
            }
        }
    }

    /* should check the checksum */
    uint8_t data = dht_data[0] + dht_data[1] + \
        dht_data[2] + dht_data[3];

    if (data != dht_data[4]) {
        DPRINTF("DHT: Checksum error: %02X %02X %02X %02X %02X\n\r",
            dht_data[0], dht_data[1], dht_data[2],
            dht_data[3], dht_data[4]);
        return FALSE;
    }

    for(int i=0; i<5; i++) {
        DPRINTF("%02X ", dht_data[i]);
    }
    DPRINTF("\n\r");

    /* DHT11 format */
    if(DHT_SENSOR_MODEL == SENSOR_MODEL_DHT11) {
        DPRINTF("Humidity:    %0d.%02d percent\n\r", dht_data[0], dht_data[1]);
        DPRINTF("Temperature: %0d.%02d degrees C\n\r", dht_data[2], dht_data[3]);
    }

    if(DHT_SENSOR_MODEL == SENSOR_MODEL_DHT22) {
        uint16_t h, t;
        h = dht_data[0] << 8 | dht_data[1];
        t = dht_data[2] << 8 | dht_data[3];

        /* DHT22 (tenths) */
        DPRINTF("Hum: %0.2f\n\r", (float)h/10.0);
        DPRINTF("Temp: %0.2f\n\r", ((float)t/10.0) * (9.0/5.0) + 32);
    }

    return TRUE;
}

uint16_t dht_get_rh(void) {
    return dht_data[0] << 8 | dht_data[1];
}

uint16_t dht_get_temp(void) {
    return dht_data[2] << 8 | dht_data[3];
}
