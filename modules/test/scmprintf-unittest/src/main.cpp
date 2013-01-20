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

#define NEWLIB_VERBOSE 1

#include <chip.h>
#include <board.h>
#include <scmRTOS.h>
#include <stdio.h>
#include <lpc_serial.h>
#include <string.h>
#include <scmprintf.h>
#include <stdlib.h>



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr2, 1000> TProcSend;
typedef OS::process<OS::pr4, 300> TProc2;
typedef OS::process<OS::pr0, 300> TProc3 ; //highest
typedef OS::process<OS::pr3, 500> TProcReceive; // lowest

//---------------------------------------------------------------------------
//      Process objects
//
TProcSend ProcSend;
TProc2 Proc2;
TProc3 Proc3;
TProcReceive ProcReceive;

//---------------------------------------------------------------------------
tick_count_t tick_count;        // global variable for OS::GetTickCount testing
int i = 0, j = 0,k = 0,l=0,m=0;

using namespace SERIAL;
using namespace IO;

Serial* serial;

int main()
{

    board_setup();
    LpcSerial lpcserial(0,BAUD_115200);
    LED4::On();

    serial = &lpcserial;
    // run
    serial->writeLine("scmprintf-unittest: starting\r\n");

    ScmPrintf::init(serial);

    LED1::On();
    LED2::On();

    OS::run();
}


namespace OS
{
    template <>
    OS_PROCESS void TProcSend::exec()
    {
        while(1)
        {
             int32_t tick_count = OS::get_tick_count();
             char* text =  "Task1: tick count: %4d \r\n";
             scmprintf(text, (tick_count)  );
             LED1::Cpl();
             sleep(2000);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        for(;;) {
            int32_t tick_count = OS::get_tick_count();
            float f = tick_count/1000 *3.1415;
            char* text = "Task2: float: %6f . Tick: %d\r\n";
            scmprintf(text, (f), tick_count  );
			sleep(900);

			void * base = malloc ( 1024 );
            scmprintf("Task2: Allocated 1k of memory, base = %04X", (int32_t) base);
			sleep(1000);
        	LED2::Cpl();
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {
        for(;;) {
        	int8_t d[6];
        	int8_t count = 0;
        	d[0] = 0;
            for(;;) {
            	for (int i = 0; i < 6;i++)
            		d[i] = i + count++;
                int32_t t = OS::get_tick_count();
                char* text = "Task3: tick count: %4d -\r\n";
                scmprintf("Task3: 6 int8's: %2x %2x %2x %2x %2x %2x\r\n",d[0],d[1],d[2],d[3],d[4],d[5]);

                sleep( t & 0x800  ?  2011  :  3171  );
            	LED3::Cpl();
            }
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
    if (j++ > 1000000)
    {
    	j = 0;
    }

}
#endif




