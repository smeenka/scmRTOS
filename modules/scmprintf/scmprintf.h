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

#ifndef SCMPRINTF_H_
#define SCMPRINTF_H_

//#include <lpc_types.h>
#include <scmRTOS.h>
#include <scmRTOS_CONFIG.h>
#include <serial.h>

#define  scmprintf(text,...) IO::ScmPrintf::print(text,__VA_ARGS__)
#ifdef DEBUG
#define  scmdebugf(text,...) IO::ScmPrintf::print(text,__VA_ARGS__)
#else
#define  scmdebugf(text,...)
#endif

namespace IO
{



class ScmPrintf{

public:
	typedef enum  {
    	dfloat_d16,
    	dfloat_d32,
    	d32_d16,
    	dfloat_x2,
    	d32_x2,
    	d16_x4,
       	d8_x8,
       	dstring_x2,
        dnone
    }TMessage;



    typedef struct {
    	char* text;
    	union{
    		struct{
    			float dfloat;
    			int16_t d16[2];
    		}dfloat_d16;
    		struct{
    			float dfloat;
    			int32_t d32;
    		}dfloat_d32;
       		struct{
       			int32_t d32;
       			int16_t d16[2];
       		}d32_d16;
       		float   dfloat_x2[2];
       		int32_t d32_x2[2];
       		int16_t d16_x4[4];
       		uint8_t  d8_x8[8];
       		char*   dstring_x2[2];
    	};
    	TMessage type;
	}TDataItem;

	typedef OS::process<SCMPRINTF_PROCESS_PRIO, SCMPRINTF_PROCESS_STACK> TProc;
	typedef OS::channel<TDataItem, SCMPRINTF_PROCESS_QUEUE,uint8_t>      TQueue;

	static void init(SERIAL::Serial* serial);

	static void print(char* text, const char* dstring0 = "",const char* dstring1 = "" );
	static void print(char* text,int32_t d32, int16_t d0,int16_t d1 = 0);
	static void print(char* text,float dfloat, int16_t d0,int16_t d1 = 0);
	static void print(char* text,float dfloat, int32_t d0);
	static void print(char* text,float d0, float d1 = 0);
	static void print(char* text,int32_t d0, int32_t d1 = 0);
	static void print(char* text,int16_t d0,int16_t d1 = 0,int16_t d2 = 0,int16_t d3 = 0);
	static void print(char* text, uint8_t d0, uint8_t d1 = 0, uint8_t d2 = 0, uint8_t d3 = 0, uint8_t d4 = 0, uint8_t d5 = 0, uint8_t d6 = 0, uint8_t d7 = 0);

	static TQueue scmQueue;
	static SERIAL::Serial* pserial;
	static SERIAL::TSerialBuffer serialBuffer;
	static OS::TEventFlag eventSend;

private:
	static TProc scmTask;
	static uint8_t dataSend[];


}; // class ScmPrintf

} // namespace IO
#endif /*SCMPRINTF_H_*/
