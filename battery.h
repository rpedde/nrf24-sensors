/*
 * battery.h Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#ifndef _BATTERY_H_
#define _BATTERY_H_

#ifndef BATTERY_VCC
# define BATTERY_VCC 3.3
#endif

#if defined(__AVR_AT90USB1286__)
# define ADC_PORT PORTF
# define ADC_DDR  DDRF
# define ADMUX_INTERNAL _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1)
#else
# error Unhandled ADC port
#endif

extern void battery_init(void);
extern void battery_sleep(void);
extern void battery_wake(void);
extern float battery_get(void);

#endif /* _BATTERY_H_ */
