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



void UART0_PrintString(SERIAL::Serial* ser,char*  s);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr2, 1000> TProc1;
typedef OS::process<OS::pr1, 300> TProc2;
typedef OS::process<OS::pr0, 300> TProc3 ; //highest
typedef OS::process<OS::pr3, 500> TProc4; // lowest

//---------------------------------------------------------------------------
//      Process objects
//
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
TProc4 Proc4;

//---------------------------------------------------------------------------
tick_count_t tick_count;        // global variable for OS::GetTickCount testing
int i = 0, j = 0,k = 0,l=0,m=0;


SERIAL::Serial* serial;

int main()
{

    board_setup();
    LED4::On();
    SERIAL::LpcSerial lpcserial(0,SERIAL::BAUD_115200);
    serial = &lpcserial;
    // run
    OS::run();
}


namespace OS
{
    template <>
    OS_PROCESS void TProc1::exec()
    {
        while(1)
        {
//            LED1::Cpl();
            int tick_count = OS::get_tick_count();
            UART0_PrintString(serial,"Task1: in the beginning of time\r\n");
            sleep(1000);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        for(;;) {
            LED2::Cpl();
            UART0_PrintString(serial,"Task2: God created in 7 days\r\n");
            sleep(300);
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {
        for(;;) {
            LED3::Cpl();
            tick_count = OS::get_tick_count();

            UART0_PrintString(serial,".");
            sleep( 11);
        }
    }

    template <>
    OS_PROCESS void TProc4::exec()
    {
    	char buffer[110];
    	int count = 0;
    	char * cp = buffer;
        for(;;) {
        	if (serial->readable()){
        		LED1::Cpl();
        		*cp = serial->read();

				if (count++ > 100  || *cp == 10){
					count = 0;
					*cp = 0;
					cp = buffer;
					UART0_PrintString(serial,buffer);
				}
				cp++;
			}else{
        		sleep(10);
        	}
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
//    	LED2::Cpl();
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


