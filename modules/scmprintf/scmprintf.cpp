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
#include <scmprintf.h>
#include <scmRTOS.h>
#include <string.h>

using namespace IO;

// initialize our static members of the clss
ScmPrintf::TQueue 		ScmPrintf::scmQueue;
SERIAL::Serial* 		ScmPrintf::pserial;
SERIAL::TSerialBuffer 	ScmPrintf::serialBuffer;
OS::TEventFlag 			ScmPrintf::eventSend;

ScmPrintf::TProc 		ScmPrintf::scmTask;
uint8_t 				ScmPrintf::dataSend[SCMPRINTF_SEND_BUFFER];



void ScmPrintf::init(SERIAL::Serial* serial) {
	pserial = serial;
	serialBuffer.size = SCMPRINTF_SEND_BUFFER;
	serialBuffer.data = dataSend;
	serialBuffer.stopchar = 10;
	serialBuffer.channel = 0;
	serialBuffer.event = &eventSend;
	serial->enableInterruptTx();

}

void ScmPrintf::print(char* message, int32_t d32, int16_t d0, int16_t d1) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.d32_d16.d32 = d32;
	item.d32_d16.d16[0] = d0;
	item.d32_d16.d16[1] = d1;
	item.type = d32_d16;
	ScmPrintf::scmQueue.push(item);
}
void ScmPrintf::print(char* message, float dfloat, int16_t d0,int16_t d1) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.dfloat_d16.dfloat = dfloat;
	item.dfloat_d16.d16[0] = d0;
	item.dfloat_d16.d16[1] = d1;
	item.type = dfloat_d16;
	ScmPrintf::scmQueue.push(item);
}
void ScmPrintf::print(char* message, float dfloat, int32_t d0) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.dfloat_d32.dfloat = dfloat;
	item.dfloat_d32.d32    = d0;
	item.type = dfloat_d32;
	ScmPrintf::scmQueue.push(item);
}
void ScmPrintf::print(char* message, const char* str0,const char* str1){
	ScmPrintf::TDataItem item;
	item.text = message;
	item.dstring_x2[0] = const_cast<char *>( str0);
	item.dstring_x2[1] = const_cast<char *>( str1);
	item.type = dstring_x2;
	ScmPrintf::scmQueue.push(item);
}


void ScmPrintf::print(char* message, float d0, float d1) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.dfloat_x2[0] = d0;
	item.dfloat_x2[1] = d1;
	item.type = dfloat_x2;
	ScmPrintf::scmQueue.push(item);
}

void ScmPrintf::print(char* message, int32_t d0, int32_t d1) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.d32_x2[0] = d0;
	item.d32_x2[1] = d1;
	item.type = d32_x2;
	ScmPrintf::scmQueue.push(item);

}


void ScmPrintf::print(char* message, int16_t d0, int16_t d1 , int16_t d2 ,int16_t d3) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.d16_x4[0] = d0;
	item.d16_x4[1] = d1;
	item.d16_x4[2] = d2;
	item.d16_x4[3] = d3;
	item.type = d16_x4;
	ScmPrintf::scmQueue.push(item);

}
void ScmPrintf::print(char* message, uint8_t d0, uint8_t d1, uint8_t d2,
		uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {
	ScmPrintf::TDataItem item;
	item.text = message;
	item.d8_x8[0] = d0;
	item.d8_x8[1] = d1;
	item.d8_x8[2] = d2;
	item.d8_x8[3] = d3;
	item.d8_x8[4] = d4;
	item.d8_x8[5] = d5;
	item.d8_x8[6] = d6;
	item.d8_x8[7] = d7;
	item.type = d8_x8;
	ScmPrintf::scmQueue.push(item);
}


namespace OS{

template<>
OS_PROCESS void ScmPrintf::TProc::exec() {
	// be sure to not start directly printing
	sleep(10);
	while (1) {
		ScmPrintf::TDataItem item;
		item.text = (char*) 0;
		item.type = IO::ScmPrintf::dnone;
		ScmPrintf::scmQueue.pop(item);
		switch (item.type) {
		case ScmPrintf::dfloat_d32:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.dfloat_d32.dfloat, item.dfloat_d32.d32);
			break;
		case ScmPrintf::dfloat_d16:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.dfloat_d16.dfloat, item.dfloat_d16.d16[0], item.dfloat_d16.d16[1]);
			break;
		case ScmPrintf::d32_d16:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.d32_d16.d32, item.d32_d16.d16[0],item.d32_d16.d16[1]);
			break;
		case ScmPrintf::dfloat_x2:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.dfloat_x2[0], item.dfloat_x2[1]);
			break;
		case ScmPrintf::d32_x2:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.d32_x2[0], item.d32_x2[1]);
			break;
		case ScmPrintf::d16_x4:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.d16_x4[0], item.d16_x4[1], item.d16_x4[2],item.d16_x4[3]);
			break;
		case ScmPrintf::d8_x8:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text, item.d8_x8[0], item.d8_x8[1], item.d8_x8[2],
					item.d8_x8[3], item.d8_x8[4], item.d8_x8[5], item.d8_x8[6], item.d8_x8[7]);
			break;
		case ScmPrintf::dstring_x2:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text,item.dstring_x2[0],item.dstring_x2[1]);
			break;
		case ScmPrintf::dnone:
			snprintf((char*) ScmPrintf::serialBuffer.data, SCMPRINTF_SEND_BUFFER-1,
					item.text);
			break;
		}
		ScmPrintf::serialBuffer.data[SCMPRINTF_SEND_BUFFER-1] = 0;
		ScmPrintf::serialBuffer.size = strlen((char*) ScmPrintf::serialBuffer.data);
		ScmPrintf::pserial->transmit(&ScmPrintf::serialBuffer);
		ScmPrintf::eventSend.wait(100);
	}
}//TProc::exec
} // namespace IO

