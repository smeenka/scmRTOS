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
#include <scmprintf.h>
#include <dispatcher.h>



//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr4, 1000> TProcSend; // lowest
typedef OS::process<OS::pr1, 500> TProcChannel0; // highes
typedef OS::process<OS::pr2, 500> TProcChannel1;
typedef OS::process<OS::pr3, 500> TProcChannel2;

//---------------------------------------------------------------------------
//      Process objects
//
TProcSend procSend;
TProcChannel0 procChannel0;
TProcChannel1 procChannel1;
TProcChannel2 procChannel2;

//---------------------------------------------------------------------------
tick_count_t tick_count;        // global variable for OS::GetTickCount testing
int i = 0, j = 0,k = 0,l=0,m=0;

OS::TEventFlag event0,event1,event2;


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
    serial->writeLine("dispatcher-unittest: starting\r\n");

    ScmPrintf::init(serial);
    Dispatcher::init(serial);
    Dispatcher::registerListener(0,&event0);
    Dispatcher::registerListener(1,&event1);
    Dispatcher::registerListener(2,&event2);

    LED1::On();
    LED2::On();
    LED3::On();

    OS::run();
}


namespace OS
{
    template <>
    OS_PROCESS void TProcSend::exec()
    {
        while(1)
        {
             char* text =  "Task1: Please enter some text. Tick count: %4d -\r\n";
             int32_t tick_count = OS::get_tick_count();
             ScmPrintf::print(text, (tick_count)  );
             sleep(10000);
        }
    }



    template <>
    OS_PROCESS void TProcChannel0::exec()
    {
        for(;;) {
        	char* data = Dispatcher::readLine(0);
            ScmPrintf::print( "Received on channel 0:");
            ScmPrintf::print( data);
            ScmPrintf::print( "\n");
            LED1::Cpl();
        }
    }
    template <>
    OS_PROCESS void TProcChannel1::exec()
    {
        for(;;) {
        	event1.wait();
        	TSerialBuffer* buffer = Dispatcher::getBuffer(1);
            ScmPrintf::print( "Received on channel 1: ");
            ScmPrintf::print( (char*) buffer->data);
            ScmPrintf::print( "\n");
            LED2::Cpl();
        }
    }
    template <>
    OS_PROCESS void TProcChannel2::exec()
    {
        for(;;) {
        	event2.wait();
        	TSerialBuffer* buffer = Dispatcher::getBuffer(2);
            ScmPrintf::print( "Received on channel 2: ");
            ScmPrintf::print( (char*) buffer->data);
            ScmPrintf::print( "\n");
            LED3::Cpl();
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




