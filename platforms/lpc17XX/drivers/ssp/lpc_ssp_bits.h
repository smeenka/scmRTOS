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



#ifndef LPC_SSP_BITS_H
#define LPC_SSP_BITS_H


//clock settings
#define PCLK_SSP1_MASK (1<< 21)  //    PCLK_peripheral = CCLK/2
#define PCLK_SSP0_MASK (1<< 11)  //    PCLK_peripheral = CCLK/2

// power settings
#define PCONP_PCSSP0 (1<<21)
#define PCONP_PCSSP1 (1<<10)





#define SSP_CR0_DSS			(1<<0)  // lowest 4 bits are the data width of the transmission
#define SSP_CR0_FRF 		(1<<5)
#define SSP_CR0_CPOL 		(1<<6)
#define SSP_CR0_CPHA  	    (1<<7)
#define SSP_CR0_PRESCALE    (1<<8)
#define SSP_CR1_LOOPBACK    (1<<0)  // 1 = loopback mode
#define SSP_CR1_ENABLE    	(1<<1)  // 1 = enabled 0 = disabled
#define SSP_CR1_SLAVE    	(1<<2)  // 0 =  master  1 =  slave
#define SSP_CR1_SOD    		(1<<3)	// 1 = slave data output disable
#define SSP_STAT_TFE   	 	(1<< 0) 	// 1= transmit fifo empty
#define SSP_STAT_TNF   		(1<< 1)	// 1= transmit fifo not full
#define SSP_STAT_RNE   		(1<< 2)	// 1= receive fifo not empty
#define SSP_STAT_RFF   		(1<< 3)	// 1= receive fifo  full
#define SSP_STAT_BUSY  		(1<< 4)	// 1= device is busy


/* SSP Interrupt Mask Set/Clear register */
#define SSP_IMSC_RORIM	(1 << 0)   // receive fifo overflow
#define SSP_IMSC_RTIM	(1 << 1)   // receive timeout
#define SSP_IMSC_RXIM	(1 << 2)   // rx fifo half full
#define SSP_IMSC_TXIM	(1 << 3)   // tx fifo half empty

/* SSP_0 Interrupt Status register */
#define SSP_RIS_RORRIS	(1 << 0)
#define SSP_RIS_RTRIS	(1 << 1)
#define SSP_RIS_RXRIS	(1 << 2)
#define SSP_RIS_TXRIS	(1 << 3)

/* SSP_0 Masked Interrupt register */
#define SSP_MIS_RORMIS	(1 << 0)  // rc fifo overflow
#define SSP_MIS_RTMIS	(1 << 1)  // rf fifo time out
#define SSP_MIS_RXMIS	(1 << 2)  // rx fifo half full
#define SSP_MIS_TXMIS	(1 << 3)  // tx fifo half empty

/* SSP_0 Interrupt clear register */
#define SSP_ICR_RORIC	(1 << 0)
#define SSP_ICR_RTIC		(1 << 1)



#endif
