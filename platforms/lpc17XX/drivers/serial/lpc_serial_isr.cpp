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

#include <lpc_serial.h>
#include <chip.h>
#include "lpc_serial_bits.h"

#ifndef DISPATCHER_NR_CHANNELS
#define DISPATCHER_NR_CHANNELS 1
#endif

using namespace SERIAL;

	// allocate the static member of the class
	LpcSerial* LpcSerial::isrVectorTable[MAX_SERIAL_DEVICES];

    /* Function: enableInterruptTX
     *  Enable the interrupt driven TX flow
     *
     * Variables:
     */
	void LpcSerial::enableInterruptTx(){
		LpcSerial::registerIsr(deviceNr,this);
		pUart->IER |=  UART_IER_THREINT_EN;
	}
	void LpcSerial::disableInterruptTx(){
		pUart->IER &=  ~UART_IER_THREINT_EN;

	}

	/* Function: enableInterruptRx
     *  Enable the interrupt driven RX flow
     *
     * Variables:
     */
	void LpcSerial::enableInterruptRx(){
		LpcSerial::registerIsr(deviceNr,this);
		pUart->IER |= UART_IER_RBRINT_EN ;
	}
	void LpcSerial::disableInterruptRx(){
		pUart->IER &= ~UART_IER_RBRINT_EN ;
	}


	/*
	 * Function: transmit
	 * transmit a buffer of data via the uart, in non-blocking way, with interrupt help
	 * if the buffer is fully transmitted, call the CB_BufferReady, if registered
	 * Variables:
	 *      txbuf:	 pointer to transmit buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case the previous transmit was not completed yet
	 */
	int8_t LpcSerial::transmit(TSerialBuffer *txbuf){
		// check that no transmit is pending
		if (sendBuffer != 0) return 0;
		sendBuffer = txbuf;
		sendBuffer->index = 0;

		uint8_t fifo_cnt = UART_TX_FIFO_SIZE;
		// send channel number
		if(txbuf->channel > 0){
			pUart->THR = txbuf->channel;
			fifo_cnt--;
		}
		// start sending the buffer
		while (fifo_cnt-- && (sendBuffer->index <  sendBuffer->size)) {
			uint8_t c = sendBuffer->data[sendBuffer->index++];
			pUart->THR = c;
			// if the stop character is send, stop here
			if( (sendBuffer->stopchar > 0) && (sendBuffer->stopchar == c) ){
				sendBuffer->index = sendBuffer->size;
			}
		}
		return -1;
	}


	/*
	 * Function: receive
	 * receive a buffer of data via the uart, in non-blocking way, with interrupt help
	 * if the buffer is fully received, call the CB_BufferReady, if registered
	 * Variables:
	 *      rxbuf:	 pointer to receive buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case a receive is still in progress
	 */
	int8_t LpcSerial::receive(TSerialBuffer *rxbuf){
		// check that no receive is pending
		if (receiveBuffer != 0) return 0;
		receiveBuffer = rxbuf;
		receiveBuffer->index = 0;
		receiveBuffer->channel = 0; // default channel 0
		return -1;
	}




	// static function to add the this pointer to the static table
    void LpcSerial::registerIsr(int dev, LpcSerial* thisInstance){
    	isrVectorTable[dev] = thisInstance;
    	uint8_t intnr = dev + UART0_IRQn;
     	NVIC->IP[1] |= 0xF000; // Middle priority
    	NVIC->ISER[0] = (1 << intnr);
    }


 /*
  * C interrupt handlers calling C++ handlers
  */
    OS_INTERRUPT void UART0_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSerial::isrVectorTable[0]->isr();
	}

    OS_INTERRUPT void UART1_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSerial::isrVectorTable[1]->isr();
	}

    OS_INTERRUPT void UART2_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSerial::isrVectorTable[2]->isr();
	}

    OS_INTERRUPT void UART3_IRQHandler(void)
	{
    	OS::TISRW ISRW;
		LpcSerial::isrVectorTable[3]->isr();
	}

/*
 * Interrupt handler
 */
	void LpcSerial::isr(){

		uint32_t intsrc, tmp, tmp1;


		/* Determine the interrupt source */
		intsrc = pUart->IIR;
		tmp = intsrc & UART_IIR_INTID_MASK;


		// Receive Line Status
		if ( (tmp & UART_IIR_INTID_RLS) == UART_IIR_INTID_RLS){
			// Check line status
			tmp1 = pUart->LSR;
			// Mask out the Receive Ready and Transmit Holding empty status
			tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
					| UART_LSR_BI | UART_LSR_RXFE);
			// If any error exist
			if (tmp1) {
				// Call Call-back function with error input value
			}
		}

		// Receive Data Available or Character time-out
		if (( (tmp & UART_IIR_INTID_RDA) == UART_IIR_INTID_RDA) || ( (tmp & UART_IIR_INTID_CTI) == UART_IIR_INTID_CTI)){
			// proceed recieveing the buffer
			while (pUart->LSR & LSR_RDR && (receiveBuffer->index < receiveBuffer->size) ) {
				uint8_t c = pUart->RBR;
				if (receiveBuffer->index == 0 && c < DISPATCHER_NR_CHANNELS ){
					receiveBuffer->channel = c;
				}else{
					// if the stop character is receive stop here, replace stopchar with string termination
					if( (receiveBuffer->stopchar > 0) && (receiveBuffer->stopchar == c) ){
						receiveBuffer->data[receiveBuffer->index] = 0;
						receiveBuffer->index = receiveBuffer->size;
					}else{
						receiveBuffer->data[receiveBuffer->index++] = c;
					}
				}
			}
			OS::TEventFlag* pevent =  receiveBuffer->event;
			if ( (receiveBuffer->index == receiveBuffer->size) && pevent){
				receiveBuffer = 0;
				pevent->signal_isr();
			}
		}

		// Transmit Holding Empty
		if ( (tmp & UART_IIR_INTID_THRE)== UART_IIR_INTID_THRE && sendBuffer){
			OS::TEventFlag* pevent =  sendBuffer->event;
			if ( (sendBuffer->index ==  sendBuffer->size) && pevent){
				sendBuffer = 0;
				pevent->signal_isr();
			}else{
				uint8_t fifo_cnt = UART_TX_FIFO_SIZE;
				// proceed sending the buffer
				while (fifo_cnt-- && (sendBuffer->index <  sendBuffer->size) ) {
					uint8_t c = sendBuffer->data[sendBuffer->index++];
					pUart->THR = c;
					// if the stop character is send stop here
					if( (sendBuffer->stopchar > 0) && (sendBuffer->stopchar == c) ){
						sendBuffer->index =  sendBuffer->size;
					}
				}
			}
		}

		intsrc &= (UART_IIR_ABEO_INT | UART_IIR_ABTO_INT);
		// Check if End of auto-baudrate interrupt or Auto baudrate time out
		if (intsrc){
			// Clear interrupt pending
			pUart->ACR |= ((intsrc & UART_IIR_ABEO_INT) ? UART_ACR_ABEOINT_CLR : 0) \
							| ((intsrc & UART_IIR_ABTO_INT) ? UART_ACR_ABTOINT_CLR : 0);
		}
	}
