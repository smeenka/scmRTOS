/**********************************************************************
* $Id$		ea1788_board.c			2011-11-20
*//**
* @file		ea1788_board.c
* @brief	Board specific functions for the EA1788 board
* @version	1.0
* @date		20. Nov. 2011
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#include <chip.h>
#include <board.h>



void board_setup(void)
{
    LED1::Init();
    LED2::Init();
    LED3::Init();
    LED4::Init();
}

/* Populates the MAC address for the board */
void board_get_macaddr(uint8_t *macaddr)
{
	macaddr[0] = LPC_EMAC_ADDR0;
	macaddr[1] = LPC_EMAC_ADDR1;
	macaddr[2] = LPC_EMAC_ADDR2;
	macaddr[3] = LPC_EMAC_ADDR3;
	macaddr[4] = LPC_EMAC_ADDR4;
	macaddr[5] = LPC_EMAC_ADDR5;
}

/* --------------------------------- End Of File ------------------------------ */
