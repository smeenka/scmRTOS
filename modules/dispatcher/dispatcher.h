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

#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <lpc_types.h>
#include <scmRTOS.h>
#include <scmRTOS_CONFIG.h>
#include <serial.h>

#define scmreadline(channel) IO::Dispatcher::readLine(channel)

using namespace SERIAL;



namespace IO
{

class Dispatcher{
public:
	typedef struct{
		OS::TEventFlag* event;
		TSerialBuffer* serialBuffer;
	} TChannel;

	typedef OS::process<DISPATCHER_PROCESS_PRIO, DISPATCHER_PROCESS_STACK> TProc;

	static void init(SERIAL::Serial* serial);

	/* Function: registerListener
     *  For given chanel register the listener
     *
     * Variables:
     *  event:	scm eventflag, owned by caller
     *  returns - 1 on success, 0 on error
     */

	static int8_t registerListener(uint8_t channel, OS::TEventFlag* event);

	/* Function: getBuffer
     *  This function should be called, after the caller is signaled that the buffer is filled.
     *
     * Variables:
     *  channel:	channel to fetch the buffer from
     *  returns - 1 on success, 0 on error
     */
	static TSerialBuffer* getBuffer(uint8_t channel = 0);

	/* Function: readLine
     *  block the caller, until a line of data (marked by stopchar) is received on given channel
     *
     * Variables:
     *  returns - ponter to received string on success, 0 on error
     */
	static char* readLine(uint8_t channel = 0);


	static Serial* pserial;
	static TSerialBuffer serialBuffers[];
	static uint8_t dataBuffers[][DISPATCHER_RCV_BUFFER];
	static TChannel channels[];
	static TProc proc;
	static uint8_t bufferIndex;
	static OS::TEventFlag event;

};
} // namespace IO
#endif /*DISPATCHER_H_*/
