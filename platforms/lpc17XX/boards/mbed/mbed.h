/*****************************************************************************
			 /-o-o-o
	   -o-o-/--o-o-o

Copyright 2012 Anton Smeenk

This file is part of project scmRTOS_CortexM3.

scmRTOS_CortexM3 is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.
scmRTOS_CortexM3 is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
scmRTOS_CortexM3. If not, see http://www.gnu.org/licenses/.
*****************************************************************************/

#ifndef BOARD_MBED_H_
#define BOARD_MBED_H_

#include "LPC17xx.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_timer.h"
#include "Pin.h"


typedef Pin<'1',18,'H',OUTPUT>	LED1;            /* Led1 */
typedef Pin<'1',20,'H',OUTPUT>	LED2;           /* Led2 */
typedef Pin<'1',21,'H',OUTPUT>	LED3;            /* Led3 */
typedef Pin<'1',23,'H',OUTPUT>	LED4;             /* Led4 */


#endif // BOARD_MBED_H_
//-----------------------------------------------------------------------------

