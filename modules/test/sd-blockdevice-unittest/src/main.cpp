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



#include <chip.h>
#include <board.h>
#include <scmRTOS.h>
#include <type.h>
#include <stdio.h>
#include <lpc_serial.h>
#include <string.h>
#include <devman.h>
#include <scmprintf.h>
#include <diskio.h>
#include <lpc_ssp.h>



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr1, 3000> TProcSend;
typedef OS::process<OS::pr4, 300> TProc2;
typedef OS::process<OS::pr2, 300> TProc3 ; //highest
typedef OS::process<OS::pr3, 300> TProcReceive; // lowest

//---------------------------------------------------------------------------
//      Process objects
//
TProcSend ProcSend;
TProc2 Proc2;
TProc3 Proc3;
TProcReceive ProcReceive;

//---------------------------------------------------------------------------
int i = 0, j = 0,k = 0,l=0,m=0;

using namespace SERIAL;
using namespace IO;

Serial* serial;
void disk_read (DWORD sector);

int main()
{

    board_setup();
    LpcSerial lpcserial(0,BAUD_115200);
    LED4::On();

    serial = &lpcserial;
    ScmPrintf::init(serial);
    // run
    serial->writeLine("\nsd-blockdevice-unittest: starting\r\n");
    LED1::On();
    LED2::Off();

	LpcSSP s1 = LpcSSP(1,8, LpcSSP::MASTER);


    OS::run();
}

//extern void power_on();
namespace OS
{
    template <>
    OS_PROCESS void TProcSend::exec()
    {
    	//BYTE buffer[512];
        while(1)
        {
            int32_t tick_count = OS::get_tick_count();
            scmprintf("disk_initialize: tick count: %4d -\n",tick_count);
            disk_initialize(0);
            LED3::On();
            int32_t status = disk_status(0);
            scmprintf("==================disk_status: %4d -\n", status);
            LED1::Cpl();
            sleep(1000);
            disk_read(2048);
            sleep(1000);
            disk_read(10000);
            sleep(1000);
            disk_read(20000);
            sleep(10000);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        for(;;) {
			sleep(900);
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {

           for(;;) {
                sleep(10);
            }

    }

    template <>
    OS_PROCESS void TProcReceive::exec()
    {
        for(;;) {
        	sleep(10);
        }
    }

}

//---------------------------------------------------------------------------
#if  scmRTOS_SYSTIMER_HOOK_ENABLE
void OS::system_timer_user_hook()
{
    TCritSect cs;
    if (i++ >= 1000)
    {
    	i = 0;
    	LED4::Cpl();
    }
}
#endif


void OS::context_switch_user_hook(void){
}
//---------------------------------------------------------------------------
#if  scmRTOS_IDLE_HOOK_ENABLE
void OS::idle_process_user_hook()
{
    TCritSect cs;
    if (j++ > 100000)
    {
    	j = 0;
       	LED3::Cpl();
    }

}
#endif


void printchar(uint8_t c){
	if(c > 31 && c < 127 )
		scmprintf("%c",c);

	else
		scmprintf(".","");

}

void disk_read (DWORD sector){

	uint8_t buffer[512];
	uint8_t line[16];
	DRESULT result = disk_read(0,   buffer, sector,1);
    scmprintf("== sector: %d result:%d \n", (int32_t) sector,(int32_t) result);
    if(result == 0){
		int index = 0;
		for (int32_t ix = 0;ix < 32;ix++){
			for (int jx = 0; jx < 16; jx++){
				line [jx] = (uint8_t) buffer[index++];
			}
			scmprintf("%04x: ", ix * 16);
			scmprintf("%02x %02x %02x %02x %02x %02x %02x %02x -- ",line[0],line[1],line[2],line[3],line[4],line[5],line[6],line[7]);
			scmprintf("%02x %02x %02x %02x %02x %02x %02x %02x -- ",line[8],line[9],line[10],line[11],line[12],line[13],line[14],line[15]);
			for (int jx = 0; jx < 8; jx++){
				printchar(line[jx]);
			}
			scmprintf(" -- ","");
			for (int jx = 8; jx < 16; jx++){
				printchar(line[jx]);
			}
			scmprintf("\n","");
		}
    }

}
