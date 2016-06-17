/*
 * battery.c Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

/* This measures the battery level by using an ADC pin
 * measured against the internal band gap reference voltage */

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <util/delay.h>

#include "util.h"
#include "hardware.h"
#include "battery.h"

static char *label="bat: ";
static uint16_t val;
static int sample_count = BATTERY_INTERVAL;

void battery_init(void) {
    DPUTS(label); DPUTS("init"); DCR;

    /* set the adc pins to input */
    CLEARBIT(ADC_DDR, BATTERY_ADC_PIN);
    CLEARBIT(ADC_PORT, BATTERY_ADC_PIN);

    /* set the battery enable pin low */
    SETBIT(BATTERY_EN_DDR, BATTERY_EN_PIN);
    CLEARBIT(BATTERY_EN_PORT, BATTERY_EN_PIN);

    /* enable adc */
    ADCSRA = _BV(ADEN);

    _delay_ms(300);

#if F_CPU == 8000000
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);  /* 64 */
#else
#   error "Set ADC prescalar"
#endif
}

void battery_sleep(void) {
    CLEARBIT(ADCSRA, ADEN);
}

void battery_wake(void) {
}

int battery_get(void) {
    sample_count++;
    if(sample_count < BATTERY_INTERVAL) {
        return FALSE;
    }
    sample_count = 0;
    SETBIT(ADCSRA, ADEN);

    /* turn on the battery voltage divider */
    SETBIT(BATTERY_EN_PORT, BATTERY_EN_PIN);

    ADMUX = _BV(REFS0) | _BV(ADLAR) | BATTERY_ADC_PIN;

    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);

    val = ADCL;
    val |= ADCH << 8;

    DPUTS(label); DPUTWORDX(val); DCR;

    /* turn off the battery voltage divider */
    CLEARBIT(BATTERY_EN_PORT, BATTERY_EN_PIN);
    return TRUE;
}

uint16_t battery_read(void) {
    return val;
}
