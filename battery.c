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
#include <avr/delay.h>

#include "util.h"
#include "hardware.h"
#include "battery.h"

float battery_internal_vref;
static char *label="batt: ";

void battery_init(void) {
    uint8_t val;

    DPUTS(label); DPUTS("init"); DCR;

    /* set the adc pins to input */
    CLEARBIT(ADC_DDR, BATTERY_ADC_PIN);
    CLEARBIT(ADC_PORT, BATTERY_ADC_PIN);

    /* set up ADC */
    ADMUX = ADMUX_INTERNAL;
    ADMUX |= _BV(ADLAR);                /* left justified */

    /* enable adc */
    ADCSRA = _BV(ADEN);

    _delay_ms(300);

#if F_CPU == 8000000
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1);  /* 64 */
#else
# error "Set ADC prescalar"
#endif


    /* read the battery voltage to determine internal vref */
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);

    val = ADCH;

    battery_internal_vref = (val * BATTERY_VCC) / 255.0;

    ADMUX = _BV(REFS1) | _BF(REFS0)
}

void battery_sleep(void) {
}

void battery_wake(void) {
}

float battery_get(void) {
    uint8_t val;

    ADMUX &= 0b11110000;
    ADMUX |= BATTERY_ADC_PIN;

    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);

    val = ADCH;
    return 0.0;
}
