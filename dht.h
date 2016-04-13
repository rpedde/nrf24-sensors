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

#ifndef _DHT_H_
#define _DHT_H_

extern void dht_init(void);
extern void dht_sleep(void);
extern void dht_wake(void);

extern int dht_read_data(void);

extern uint16_t dht_get_temp(void);
extern uint16_t dht_get_rh(void);

/* Set up prescalar and counters for 40 and 80 us for timing */
#if F_CPU == 8000000
# define DHT_FORTY  40
# define DHT_EIGHTY 80
# define DHT_HUNDRED 100
# define DHT_CS00 0
# define DHT_CS01 1
# define DHT_CS02 0
#elif F_CPU == 16000000
# define DHT_FORTY  80
# define DHT_EIGHTY 160
# define DHT_HUNDRED 200
# define DHT_CS00 0
# define DHT_CS01 1
# define DHT_CS02 0
#else
# error Unhandled prescaler in dht.h
#endif

#if defined(__AVR_AT90USB1286__)
# define DHT_PRESCALE_REG TCCR0B
#elif defined(__AVR_ATmega32U4__)
# define DHT_PRESCALE_REG TCCR0
#elif defined(__AVR_ATtiny88__) || defined(__AVR_Attiny44__)
# define DHT_PRESCALE_REG TCCR0A
#else
# error Unhandled CPU in dht.h
#endif


#endif /* _DHT_H_ */
