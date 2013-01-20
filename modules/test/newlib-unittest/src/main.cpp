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
#include <stdio.h>
#include <lpc_serial.h>
#include <string.h>
#include <devman.h>
#include <genstd.h>



void UART0_PrintString(SERIAL::Serial* ser,char*  s);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr2, 1000> TProcSend;
typedef OS::process<OS::pr1, 800> TProc2;
typedef OS::process<OS::pr0, 1200> TProc3 ; //highest
typedef OS::process<OS::pr3, 800> TProcReceive; // lowest

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

Serial* serial;

int main()
{

    board_setup();
    LpcSerial lpcserial(0,BAUD_115200);
    LED4::On();

    serial = &lpcserial;
    // run
    UART0_PrintString(serial,"newlib-unittest: starting\r\n");

    dm_init();
    std_register();
    printf("newlib-unittest: printing with printf\r\n");
    printf("newlib-unittest: and newlib stddevice registered\r\n");
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
             char* text =  "Task1: tick count: %4d -\r\n";
             printf(text, (tick_count)  );
             LED1::Cpl();
             sleep(2000);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
    	sleep(2000);
        for(;;) {
            int32_t tick_count = OS::get_tick_count();
            int32_t f = tick_count/1000 *3.1415;
            char* text = "Task2: float: %4d . Tick: %d\r\n";
            printf(text, tick_count  );
			sleep(900);
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
 //               printf(text );
                printf("Task3: 6 int8's: %2x %2x %2x %2x %2x %2x\r\n",d[0],d[1],d[2],d[3],d[4],d[5]);

                sleep( t & 0x800  ?  2011  :  3171  );
            	LED3::Cpl();
            }
        }
    }

    template <>
    OS_PROCESS void TProcReceive::exec()
    {
    	std_register();
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
       	LED1::Cpl();
    }

}
#endif


void UART0_PrintString(SERIAL::Serial* ser,char*  s)
{
	// loop through until reach string's zero terminator
	int  ix = 0;
	while(s[ix] != 0){
		ser->write(s[ix]); // print each character
		ix++;
	}
}


