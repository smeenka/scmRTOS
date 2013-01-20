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



#ifndef LPC_SSP_H
#define LPC_SSP_H

#include <serial.h>
#include <LPC17xx.h>

#define MAX_SSP_DEVICES 2

namespace SERIAL {

typedef struct{
	uint8_t* data;
	uint16_t size;    // nr of bytes still to send or receive.
	uint16_t indexWrite;   // current write index  in the buffer
	uint16_t indexRead;    // current read index  in the buffer
	uint8_t  device;  // for device == 0  the SSEL of the ssp is used. For other devices this has to be done be the callee
	OS::TEventFlag* event;
}TSSPBuffer;

/* Class: LpcSerial
 *   Class for Serial input/output for lpc
 *
 */
class LpcSSP  {

public:
	typedef enum {
		MASTER = 0,
		SLAVE,
		MASTER_LB
	}mode_t ;
	typedef enum {
		BIT_100K = 0,
		BIT_1M,
		BIT_10M
	}speed_t ;
    /* Constructor: LpcSerial
     *  Create a SSPl port
     *
     * Variables:
     */
    LpcSSP(uint8_t deviceNr,uint8_t nrBits, mode_t mode);

    /* Destructor: LpcSerial
     */
    ~LpcSSP();

    /* Function: speed
     * Sets the clock speed of the (master) ssp
     * Variables:
     *
     *  returns - 1 on succesfull, 0 on error
     */
    uint8_t speed(speed_t bitspeed);

    /* Function: enableSSEL
     * if enabled, connects the ssel pin to the ssp device (function 2)
     * if disabled, restores the ssel pin as a generic input with pullup (function 0)
     * Variables:
     *
     *  returns - 1 on succesfull, 0 on error
     */
    uint8_t enableSSEL(uint8_t enabled);


    /********************************************************
    * Non Blocking interface, backed by interrupts
    ********************************************************/
	/*
	 * Function: interrupt driven, non blocking transfer
	 * First flush the ssp, wait until not busy, then write until tx fifo is full
	 * For a master transmit is started immediately, for a slave, wait until a master starts transmitting
	 * if the buffer is fully received, then via the eventflag the callee is informed
	 * Note that at this moment the data in the buffer is overwritten with the received data
	 * Variables:
	 *      buf:	 pointer to transmit buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case a receive is still in progress
	 */
	int8_t transfer(TSSPBuffer *buf);

	/********************************************************
	* Blocking interface
	********************************************************/


    /* Function: transfer
     *  Flush the receive fifo, and wait until fifo is not busy
     *  Write an value and receive data synchronously (in master mode)
     *  Function blocks then until data is fully send.  serial port is not busy and than writes the char
     * Variables:
     *  data - The data to write to the ssp serial port
     *  returns - the received data from ssp
     */
    uint16_t transfer(uint16_t data);

    /* Function: write
     *  Check if there is place in the send fifo. If yes, add to fifo, else wait until there is space
     *  Write an value and return
     * Variables:
     *  data - The data to write to the ssp serial port
     *  returns - -1 if  successfull, 0 in case of error
     */
    uint8_t write(uint16_t data);

    /* Function: read
     *  this function will do a transfer with as send byte 0xFF
     *  Write an value and return
     * Variables:
     *  returns - the received data
     */
    uint16_t read();

     /* Function: busy
      *  Determine if there is space available to write a character
      *
      * Variables:
      *  returns - 1 if busy 0 if not busy
      */
     int8_t busy();

     /* Function: flush
      *  Flush the receive fifo
      * Variables:
      */
     void flush();

    static LpcSSP* instances[];
    static void registerIsr(int dev, LpcSSP* thisInstance);
    void isr();


private:
    LPC_SSP_TypeDef* pSSP;
    TSSPBuffer * buffer;
    uint8_t deviceNr;
};


} // namespace


#endif
