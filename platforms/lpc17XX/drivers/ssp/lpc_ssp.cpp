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

#include <lpc_ssp.h>
#include <chip.h>
#include "lpc_ssp_bits.h"
#include <board.h>


   	 typedef Pin<'0', 18, 'H', INPUT_PULLUP, PINSEL_2> MOSI_0;
   	 typedef Pin<'0', 17, 'H', INPUT_PULLUP, PINSEL_2> MISO_0;
   	 typedef Pin<'0', 15, 'H', INPUT_PULLUP, PINSEL_2> SSCK_0;
   	 typedef Pin<'0', 16, 'H', INPUT_PULLUP, PINSEL_2> SSEL_0;
  	 typedef Pin<'0', 9, 'H', INPUT_PULLUP, PINSEL_2> MOSI_1;
  	 typedef Pin<'0', 8, 'H', INPUT_PULLUP, PINSEL_2> MISO_1;
  	 typedef Pin<'0', 7, 'H', INPUT_PULLUP, PINSEL_2> SSCK_1;
  	 typedef Pin<'0', 6, 'H', INPUT_PULLUP, PINSEL_2> SSEL_1;

using namespace SERIAL;

/* Constructor: LpcSSP
 *  Create a SSP Serial port
 *
 * Variables:
 *  nrBits - nr of bits to use for each transaction
 */
LpcSSP::LpcSSP(uint8_t dev, uint8_t nrBits, mode_t mode) {
	switch (dev) {
	case 0:
		pSSP = (LPC_SSP_TypeDef *) LPC_SSP0_BASE;

	    LPC_SC->PCONP |= PCONP_PCSSP0; 		 // power on
   	    LPC_SC->PCLKSEL0 |= PCLK_SSP0_MASK;  // set pclk as divided by 2


      	 MOSI_0::Init();
      	 MISO_0::Init();;
      	 SSCK_0::Init();;
      	 SSEL_0::Init();;
		break;
	case 1:
		pSSP = (LPC_SSP_TypeDef *) LPC_SSP1_BASE;

	    LPC_SC->PCONP |= PCONP_PCSSP1;       // power on
   	    LPC_SC->PCLKSEL0 |= PCLK_SSP1_MASK;  // set pclk as divided by 2
      	 MOSI_1::Init();
      	 MISO_1::Init();;
      	 SSCK_1::Init();;
      	 SSEL_1::Init();;
		break;
	default:
		// TODO: handle errors here
		while (1)
			;
		break;
	}
	if (nrBits > 3 && nrBits < 16) {
		pSSP->CR0 = nrBits - 1;
	} else {
		pSSP->CR0 = 7;
	}
	// frame format = spi (bits 4-5) CPOL = 0, CPHA = 0
	/* Set DSS data to 8-bit, Frame format SPI, CPOL = 0, CPHA = 0, and SCR is 15 */

	pSSP->CR1 = 0;
	switch (mode) {
	case MASTER:
		break;
	case SLAVE:
		pSSP->CR1 |= SSP_CR1_SLAVE;
		break;
	case MASTER_LB:
		pSSP->CR1 |= SSP_CR1_LOOPBACK;
		break;
	}
	pSSP->CR1 |= SSP_CR1_ENABLE;
	deviceNr = dev;
	buffer = 0;
	LpcSSP::registerIsr(deviceNr,this);
}
LpcSSP::~LpcSSP() {
	pSSP->IMSC = 0;
}

/* Function: speed
 * Sets the clock speed of the (master) ssp
 * Variables:
 *
 *  returns - 1 on succesfull, 0 on error
 */
uint8_t  LpcSSP::speed(speed_t bitspeed) {
	// Clock divider to 8
	pSSP->CR0 &= 0xFF; // reset clock prescaler
	switch (bitspeed) {
	case BIT_100K:
		pSSP->CR0 |= (70 << 8);
		/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
		pSSP->CPSR = 0x08;
		break;
	case BIT_1M:
		pSSP->CR0 |= (10 << 8);
		/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
		pSSP->CPSR = 0x08;
		break;
	case BIT_10M:
		pSSP->CR0 |= (0 << 8);
		/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
		pSSP->CPSR = 0x08;
		break;

	}
	return -1;
}
/* Function: enableSSEL
 * if enabled, connects the ssel pin to the ssp device (function 2)
 * if disabled, restores the ssel pin as a generic input with pullup (function 0)
 * Variables:
 *
 *  returns - 1 on succesfull, 0 on error
 */
uint8_t LpcSSP::enableSSEL(uint8_t enabled){
	switch (deviceNr) {
	case 0:
		typedef Pin<'0', 16, 'H'> SSEL_0;
      	if (enabled)
      		SSEL_0::Init(INPUT_PULLUP, PINSEL_2);
      	else
      		SSEL_0::Init(INPUT_PULLUP, PINSEL_0);
		break;
	case 1:
		typedef Pin<'0', 6, 'H'> SSEL_1;
      	if (enabled)
      		SSEL_1::Init(INPUT_PULLUP, PINSEL_2);
      	else
      		SSEL_1::Init(INPUT_PULLUP, PINSEL_0);
		break;
	default:
		// TODO: handle errors here
		break;
	}

}


/* Function: transfer
 *  Flush the receive fifo, and wait until fifo is not busy
 *  Write an value and receive data synchronously (in master mode)
 *  Function blocks then until data is fully send.  serial port is not busy and than writes the char
 * Variables:
 *  data - The data to write to the ssp serial port
 *  returns - the received data from ssp
 */
uint16_t  LpcSSP::transfer(uint16_t data) {

	while ( (pSSP->SR & SSP_STAT_TFE) == 0); // Block until all previous stuff is send
	flush();

	pSSP->DR = data;
	while ( (pSSP->SR & SSP_STAT_RNE) == 0); // Block until data received
	return pSSP->DR;
}
/* Function: write
 *  Check if there is place in the send fifo. If yes, add to fifo, else wait until there is space
 *  Write an value and return
 * Variables:
 *  data - The data to write to the ssp serial port
 *  returns - -1 if  successfull, 0 in case of error
 */
uint8_t LpcSSP::write(uint16_t data){
	while ((pSSP->SR & SSP_STAT_TNF) == 0) {} // wait until tx fifo is not full
	pSSP->DR = data;
	return -1;
}
/* Function: read
 *  this function will do a transfer with as send byte 0xFFFF
 * Variables:
 *  returns - the received data
 */
uint16_t LpcSSP::read(){
	return transfer(0xFF);
}

/* Function: flush
 *  Flush the receive fifo
 * Variables:
 */
void  LpcSSP::flush() {
	while ((pSSP->SR & SSP_STAT_RNE) > 0) {
		pSSP->DR;
	}
}


/* Function: busy
 *  Determine if there is space available to write a character
 *
 * Variables:
 *  returns - 1 if busy 0 if not busy
 */
int8_t LpcSSP::busy() {
	return (pSSP->SR & SSP_STAT_BUSY) > 0;
}
