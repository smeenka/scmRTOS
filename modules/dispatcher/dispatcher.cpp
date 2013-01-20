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

#include <stdio.h>
#include <dispatcher.h>
#include <scmRTOS.h>
#include <scmRTOS_CONFIG.h>

using namespace IO;
using namespace SERIAL;


// initialize our static members of the clss
Serial* 				Dispatcher::pserial;
TSerialBuffer 			Dispatcher::serialBuffers[DISPATCHER_NR_CHANNELS + 1];
uint8_t 				Dispatcher::dataBuffers[DISPATCHER_NR_CHANNELS + 1][DISPATCHER_RCV_BUFFER];
OS::TEventFlag 			Dispatcher::event;
Dispatcher::TChannel 	Dispatcher::channels[DISPATCHER_NR_CHANNELS];
Dispatcher::TProc 		Dispatcher::proc;
uint8_t 				Dispatcher::bufferIndex;


void Dispatcher::init(Serial* serial) {
	pserial = serial;

	for (int i = 0; i < DISPATCHER_NR_CHANNELS+1 ; i++){
		serialBuffers[i].size = DISPATCHER_RCV_BUFFER;
		serialBuffers[i].data = dataBuffers[i];
		serialBuffers[i].stopchar = 10;
		serialBuffers[i].channel = 0;
		serialBuffers[i].event = &event;

	}
	bufferIndex = 0;
	serial->receive(&serialBuffers[bufferIndex]);
	serial->enableInterruptRx();
}

int8_t Dispatcher::registerListener(uint8_t channel, OS::TEventFlag* ev) {
	if (channel >= DISPATCHER_NR_CHANNELS) return 0;
	channels[channel].event = ev;
	return -1;
}

TSerialBuffer* Dispatcher::getBuffer(uint8_t channel){
	return channels[channel].serialBuffer;
}

char* Dispatcher::readLine(uint8_t channel){
	if(channels[channel].event){
		channels[channel].event->wait();
		return (char* ) channels[channel].serialBuffer->data;
	}
	return 0;
}

namespace OS {

template<>
OS_PROCESS void Dispatcher::TProc::exec() {
	// be sure to not start directly printing
	while (1) {
		Dispatcher::event.wait();
		TSerialBuffer* buffer = &Dispatcher::serialBuffers[Dispatcher::bufferIndex];

		// if the first character in the buffer is indeed a character, assume no channel (channel 0)
		uint8_t channel = buffer->channel;
		if (channel > DISPATCHER_NR_CHANNELS){
			channel = 0;
		}
		Dispatcher::channels[channel].serialBuffer = buffer;


		// prepare for next receive
		Dispatcher::bufferIndex++;
		// note her that the bufferIndex can be equal to DISPATCHER_NR_CHANNELS
		// so there must be one more nr of dipatcher buffers than channels
		if (Dispatcher::bufferIndex > DISPATCHER_NR_CHANNELS) Dispatcher::bufferIndex = 0;

		TSerialBuffer* next = &Dispatcher::serialBuffers[Dispatcher::bufferIndex];
		Dispatcher::pserial->receive(next);

		// signal channel that a buffer is available
		if (Dispatcher::channels[channel].event){
			Dispatcher::channels[channel].event->signal();
		}

	}
}
}

