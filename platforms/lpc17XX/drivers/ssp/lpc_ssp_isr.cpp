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


using namespace SERIAL;

	// allocate the static member of the class
	LpcSSP* LpcSSP::instances[MAX_SSP_DEVICES];



	/*
	 * Function: interrupt driven, non blocking transfer
	 * First flush the ssp, then write until tx fifo is full
	 * For a master transmit is started immediately, for a slave, wait until a master starts transmitting
	 * if the buffer is fully received, then via the eventflag the callee is informed
	 * Note that at this moment the data in the buffer is overwritten with the received data
	 * Variables:
	 *      buf:	 pointer to transmit buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case a receive is still in progress
	 */
	int8_t LpcSSP::transfer(TSSPBuffer *buf){
		// check that no transmit is pending
		if (buffer != 0) return 0;
		flush();
		while (busy()); // Block until finished with previous stuf

		buffer = buf;
		buffer->indexWrite = 0;
		buffer->indexRead = 0;

		// start sending the buffer
		while ((pSSP->SR & SSP_STAT_TNF) && (buffer->indexWrite <  buffer->size) ) {
			uint16_t c = buffer->data[buffer->indexWrite];
			buffer->data[buffer->indexWrite++] = 0;
			pSSP->DR = c;
		}
		pSSP->IMSC |=  (SSP_IMSC_RORIM | SSP_IMSC_RTIM | SSP_IMSC_RXIM); // enable interrupt
		return -1;
	}



	// static function to add the this pointer to the static table
    void LpcSSP::registerIsr(int dev, LpcSSP* thisInstance){
    	instances[dev] = thisInstance;
    	switch (dev){
    	case 0:
    		NVIC->IP[3] |= (1 << 23); // Middle priority
    		NVIC->ISER[0] = (1 << SSP0_IRQn);
    		break;
    	case 1:
    		NVIC->IP[3] |= (1 << 31) ; // Middle priority
    		NVIC->ISER[0] = (1 << (SSP0_IRQn + 1) );
    		break;
    	}
    }


 /*
  * C interrupt handlers calling C++ handlers
  */
    OS_INTERRUPT void SSP0_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSSP::instances[0]->isr();
	}

    OS_INTERRUPT void SSP1_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSSP::instances[1]->isr();
	}


/*
 * Interrupt handler
 */
	void LpcSSP::isr(){
		uint8_t finished = 0;
		uint32_t status  = pSSP->MIS;
		// clear interrup reason in case of frame overrun
		if (status & SSP_MIS_RORMIS) {
			pSSP->ICR = SSP_MIS_RORMIS;
		}

		// proceed sending the buffer, until all data is received
		while ((pSSP->SR & SSP_STAT_TNF) && (buffer->indexWrite <  buffer->size) ) {
			uint16_t c = buffer->data[buffer->indexWrite];
			buffer->data[buffer->indexWrite++] = 0;
			pSSP->DR = c;
		}
		// proceed receiving the buffer
		while ( (pSSP->SR & SSP_STAT_RNE) && (buffer->indexRead < buffer->size) ) {
			uint16_t c = pSSP->DR;
			buffer->data[buffer->indexRead++] = c;
		}
		if ( buffer->indexRead == buffer->size ){
			finished = 1;
		}
		// clear interrup reason in case of receive interrupt time out
		if (status & SSP_MIS_RTMIS) {
			pSSP->ICR = SSP_MIS_RTMIS;
			finished = 1;
		}
		if (finished){
			pSSP->IMSC &=  ~(SSP_IMSC_RORIM | SSP_IMSC_RTIM | SSP_IMSC_RXIM); //disable interrupt
			OS::TEventFlag* pevent =  buffer->event;
			buffer = 0;
			if (pevent)		pevent->signal_isr();
		}
	}
