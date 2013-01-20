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
#include <shell.h>
#include <mmcfs.h>


#include <devman.h>
#include <semifs.h>


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr4, 1000> TProcSend; // lowest
typedef OS::process<OS::pr1, 2000> TProcShell; // highes
typedef OS::process<OS::pr2, 500> TProcChannel1;
typedef OS::process<OS::pr3, 500> TProcChannel2;

//---------------------------------------------------------------------------
//      Process objects
//
TProcSend procSend;
TProcShell procChannel0;
TProcChannel1 procChannel1;
TProcChannel2 procChannel2;

//---------------------------------------------------------------------------
tick_count_t tick_count;        // global variable for OS::GetTickCount testing

OS::TEventFlag event0,event1,event2;
uint32_t i,j;


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
    serial->writeLine("device manager unittest: starting\r\n");

    ScmPrintf::init(serial);
    Dispatcher::init(serial);
    Dispatcher::registerListener(0,&event0);
    Dispatcher::registerListener(1,&event1);
    Dispatcher::registerListener(2,&event2);

    int res = dm_init();
    scmprintf( "Initializing device manager: %d\n",(int32_t)res);
    res = semifs_init();
    scmprintf( "Initializing semi fs: %d\n",(int32_t)res);
    res = mmcfs_init() ;
    scmprintf( "Initializing mmc file system: %d\n",(int32_t)res);

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
             sleep(10000);
        }
    }



    template <>
    OS_PROCESS void TProcShell::exec()
    {
        for(;;) {
            scmprintf("Starting shell on channel 0: \n","");
        	shell_start();
            LED1::Cpl();
        }
    }
    template <>
    OS_PROCESS void TProcChannel1::exec()
    {
        for(;;) {
        	event1.wait();
        	char* buffer = scmreadline(1);
            scmprintf( "Received on channel 1: %s\n",buffer);
            LED2::Cpl();
        }
    }
    template <>
    OS_PROCESS void TProcChannel2::exec()
    {
        for(;;) {
        	event2.wait();
        	char* buffer = scmreadline();
            scmprintf( "Received on channel 2: %s\n",buffer);
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
    	LED3::Cpl();
    }

}
#endif




