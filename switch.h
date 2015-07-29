/*
 * switch.h Copyright (C) 2015 Ron Pedde <ron@pedde.com>
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

#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <avr/io.h>
#include "hardware.h"

#ifndef SWITCH_INTERRUPT
# define SWITCH_INTERRUPT INT0
#endif

extern void switch_init(void);
extern void switch_sleep(void);
extern void switch_wake(void);
extern float switch_get(int which);

#endif /* _SWITCH_H_ */
