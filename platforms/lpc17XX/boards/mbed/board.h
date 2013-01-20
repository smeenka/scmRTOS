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

#ifndef _BOARD_H
#define _BOARD_H

#include <LPC17xx.h>
#include <lpc_pin.h>

/** @ingroup lpc_board
 * @{
 */


typedef Pin<'1',18,'H',OUTPUT>	LED1;           /* Led1 */
typedef Pin<'1',20,'H',OUTPUT>	LED2;           /* Led2 */
typedef Pin<'1',21,'H',OUTPUT>	LED3;           /* Led3 */
typedef Pin<'1',23,'H',OUTPUT>	LED4;           /* Led4 */





#define LPC_EMAC_ADDR0 0x00 /**< Hardware MAC address field 0 */
#define LPC_EMAC_ADDR1 0x60 /**< Hardware MAC address field 1 */
#define LPC_EMAC_ADDR2 0x37 /**< Hardware MAC address field 2 */
#define LPC_EMAC_ADDR3 0x12 /**< Hardware MAC address field 3 */
#define LPC_EMAC_ADDR4 0x34 /**< Hardware MAC address field 4 */
#define LPC_EMAC_ADDR5 0x56 /**< Hardware MAC address field 5 */


/** \brief  Setup EA1788 board for LWIP examples
 *
 *  This function sets up the LED and ethernet pin muxing for the LPC1788
 *  as used on the EA1788 board.
 */
void board_setup(void);

/** \brief  Populates the MAC address for the board.
 *
 *  This function sets the MAC address used for the EA1788 board. Although
 *  this can be replaced with something more capable, it simply uses a
 *  hard-coded MAC address for this example.
 */
void board_get_macaddr(uint8_t *macaddr);

/**
 * @}
 */

#endif /* __LPC_BOARD_H */

/* --------------------------------- End Of File ------------------------------ */
