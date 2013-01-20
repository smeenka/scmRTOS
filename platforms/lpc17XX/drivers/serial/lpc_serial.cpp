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

using namespace SERIAL;

/* Constructor: LpcSerial
 *  Create a Serial port
 *
 * Variables:
 *  baudrate - baudrate to use
 */
LpcSerial::LpcSerial(uint8_t dev, Baudrate baud) {
	switch (dev) {
	case 0:
		pUart = (LPC_UART_TypeDef *) LPC_UART0_BASE;
		break;
	case 1:
		pUart = (LPC_UART_TypeDef *) LPC_UART1_BASE;
		break;
	case 2:
		pUart = (LPC_UART_TypeDef *) LPC_UART2_BASE;
		break;
	case 3:
		pUart = (LPC_UART_TypeDef *) LPC_UART3_BASE;
		break;
	default:
		// TODO: handle errors here
		while (1)
			;
		break;
	}
	deviceNr = dev;
	setBaudrate(baud);
	sendBuffer = 0;
	receiveBuffer = 0;
}
LpcSerial::~LpcSerial() {

}

/* Function: baud
 *  Set the baud rate of the serial port
 *
 * Variables:
 *  baudrate - The baudrate of the serial port (default = 115200).
 */
void LpcSerial::setBaudrate(Baudrate baud) {
	int intbaud = baud;
	int pclk;
	unsigned long int Fdiv;

	// PCLK_UART0 is being set to 1/4 of SystemCoreClock
	pclk = SystemCoreClock / 4;

	// Turn on power to UART0
	LPC_SC->PCONP |= PCUART0_POWERON;

	// Turn on UART0 peripheral clock
	LPC_SC->PCLKSEL0 &= ~(PCLK_UART0_MASK);
	LPC_SC->PCLKSEL0 |= (0 << PCLK_UART0); // PCLK_periph = CCLK/4

	// Set PINSEL0 so that P0.2 = TXD0, P0.3 = RXD0
	LPC_PINCON->PINSEL0 &= ~0xf0;
	LPC_PINCON->PINSEL0 |= ((1 << 4) | (1 << 6));

	pUart->LCR = 0x83; // 8 bits, no Parity, 1 Stop bit, DLAB=1
	Fdiv = (pclk / 16) / intbaud; // Set baud rate
	pUart->DLM = Fdiv / 256;
	pUart->DLL = Fdiv % 256;
	pUart->LCR = 0x03; // 8 bits, no Parity, 1 Stop bit DLAB = 0
	pUart->FCR = 0x07; // Enable and reset TX and RX FIFO
	pUart->FCR |=UART_FCR_TRG_LEV3; // after 14 characters, generate a rx interrupt
}

/* Function: format
 *  Set the transmission format used by the Serial port
 *
 * Variables:
 *  bits - The number of bits in a word (5-8; default = 8)
 *  parity - The parity used (SoftwareSerial::None, SoftwareSerial::Odd, SoftwareSerial::Even, SoftwareSerial::Forced1, SoftwareSerial::Forced0; default = SoftwareSerial::None)
 *  stop - The number of stop bits (1 or 2; default = 1)
 */
/*void format(int bits = 8, Parity parity = Serial::None, int stop_bits = 1); */

/* Function: write
 *  Write a character
 *  Function blocks until serial port is not busy and than writes the char
 * Variables:
 *  c - The character to write to the serial port
 *  returns - 1 on succes, 0 on error
 */
int8_t LpcSerial::write(uint8_t c) {
	while ((pUart->LSR & LSR_THRE) == 0)
		; // Block until tx empty
	pUart->THR = c;
	return -1;
}

/* Function: writeLine
 *  Write a string, ending with a zero character
 *  Function blocks until the whole string is send
 *  Note that this function should be used only in non interrupt driven mode
 * Variables:
 *  line - The string  to write to the serial port
 *  returns - 1 on succesfull, 0 on error
 */
int8_t LpcSerial::writeLine(char* line){

	int  i = 0;
	while(line[i] != 0){
		write(line[i++]);
	}
}

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
int16_t LpcSerial::read() {
	while ((pUart->LSR & LSR_RDR) == 0)
		; // Nothing received so just block
	return pUart->RBR; // Read Receiver buffer register
}


/* Function: readable
 *  Determine if there is a character available to read
 *
 * Variables:
 *  returns - 1 if there is a character available to read, else 0
 */
int8_t LpcSerial::readable() {
	return (pUart->LSR & LSR_RDR) > 0;
}

/* Function: writeable
 *  Determine if there is space available to write a character
 *
 * Variables:
 *  returns - 1 if there is space to write a character, else 0
 */
int8_t LpcSerial::writeable() {
	return (pUart->LSR & LSR_THRE) > 0;
}


