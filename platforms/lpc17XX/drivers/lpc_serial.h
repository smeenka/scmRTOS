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



#ifndef LPC_SERIAL_H
#define LPC_SERIAL_H

#include <serial.h>
#include <LPC17xx.h>

#define MAX_SERIAL_DEVICES 4

namespace SERIAL {


/* Class: LpcSerial
 *   Class for Serial input/output for lpc
 *
 */
class LpcSerial: public Serial  {

public:

    /* Constructor: LpcSerial
     *  Create a Serial port
     *
     * Variables:
     *  baudrate - baudrate to use 
     */
    LpcSerial(uint8_t deviceNr,Baudrate baudrate);

    /* Destructor: LpcSerial
     */
    ~LpcSerial();


    /* Function: baud
     *  Set the baud rate of the serial port
     *
     * Variables:
     *  baudrate - The baudrate of the serial port (default = 115200).
     */
    void setBaudrate(Baudrate baudrate);



    /* Function: format
     *  Set the transmission format used by the Serial port
     *
     * Variables:
     *  bits - The number of bits in a word (5-8; default = 8)
     *  parity - The parity used (SoftwareSerial::None, SoftwareSerial::Odd, SoftwareSerial::Even, SoftwareSerial::Forced1, SoftwareSerial::Forced0; default = SoftwareSerial::None)
     *  stop - The number of stop bits (1 or 2; default = 1)
     */
    /*void format(int bits = 8, Parity parity = Serial::None, int stop_bits = 1); */

    /********************************************************
    * Non Blocking interface, backed by interrupts
    ********************************************************/

    /* Function: enableInterruptTX
     *  Enable the interrupt driven TX flow
     *
     * Variables:
     */
	void enableInterruptTx();
	void disableInterruptTx();

	/* Function: enableInterruptRx
     *  Enable the interrupt driven RX flow
     *
     * Variables:
     */
	void enableInterruptRx();
	void disableInterruptRx();



	/*
	 * Function: transmit
	 * transmit a buffer of data via the uart, in non-blocking way, with interrupt help
	 * if the buffer is fully transmitted, call the CB_BufferReady, if registered
	 * Variables:
	 *      txbuf:	 pointer to transmit buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case the previous transmit was not completed yet
	 */
	int8_t transmit(TSerialBuffer *txbuf);


	/*
	 * Function: receive
	 * receive a buffer of data via the uart, in non-blocking way, with interrupt help
	 * if the buffer is fully received, call the CB_BufferReady, if registered
	 * Variables:
	 *      txbuf:	 pointer to transmit buffer (owned by caller)
     *  returns - 1 on succesfull, 0 on error, in case a receive is still in progress
	 */
	int8_t receive(TSerialBuffer *txbuf);

	/********************************************************
	* Blocking interface
	********************************************************/


    /* Function: write
     *  Write a character
     *  Function blocks until serial port is not busy and than writes the char
     * Variables:
     *  c - The character to write to the serial port
     *  returns - 1 on succesfull, 0 on error
     */
    int8_t write(uint8_t c);


    /* Function: writeLine
     *  Write a string, ending with a zero character
     *  Function blocks until the whole string is send
     *  Note that this function should be used only in non interrupt driven mode
     * Variables:
     *  line - The string  to write to the serial port
     *  returns - 1 on succesfull, 0 on error
     */
    int8_t writeLine(char* line);

    /* Function: read
     *  Read a character
	 *
     * Reads a character from the serial port. This will block until
     * a character is available. To see if a character is available,
     * see <readable>
     *
     * Variables:
     *  returns - The character read from the serial port
     */
    int16_t read();





    /* Function: readable
     *  Determine if there is a character available to read
     *
     * Variables:
     *  returns - 1 if there is a character available to read, else 0
     */
     int8_t readable();

    /* Function: writeable
     *  Determine if there is space available to write a character
     *
     * Variables:
     *  returns - 1 if there is space to write a character, else 0
     */
    int8_t writeable();

    static LpcSerial* isrVectorTable[];
    static void registerIsr(int dev, LpcSerial* thisInstance);
    void isr();


private:
    LPC_UART_TypeDef* pUart;
    TSerialBuffer * receiveBuffer;
    TSerialBuffer * sendBuffer;
    uint8_t deviceNr;
};


} // namespace


#endif
