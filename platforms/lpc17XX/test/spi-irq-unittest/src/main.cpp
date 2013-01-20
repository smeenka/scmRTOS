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
#include <lpc_ssp.h>
#include <string.h>


//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr2, 1000> TProc1;
typedef OS::process<OS::pr1, 500> TProc2;
typedef OS::process<OS::pr0, 500> TProc3 ; //highest
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

using namespace SERIAL;

Serial* serial;
LpcSSP* ssp0;
LpcSSP* ssp1;

TSSPBuffer bufferMaster,bufferSlave;

char dataMaster[128];
char dataSlave[128];


OS::TEventFlag eventMaster,eventSlave;
OS::TMutex mutex;
char* text1 = "In the beginning of time";

int main()
{
    board_setup();
    LED4::On();
    LpcSerial lpcserial(0,SERIAL::BAUD_115200);
    serial = &lpcserial;
    serial->writeLine("ssp-unit irq  test: starting\r\n");
    serial->writeLine(" ssp 0 is the master, ssp1 is the slave\r\n");
    serial->writeLine(" For mbed, connect pin 8 - 14; 13 -7, 12-6, 11- 5 \r\n");

    LpcSSP s0 = LpcSSP(0,8, LpcSSP::MASTER);
    s0.speed(LpcSSP::BIT_10M);
    ssp0 = &s0;

    LpcSSP s1 = LpcSSP(1,8, LpcSSP::SLAVE);
    s1.speed(LpcSSP::BIT_10M);
    ssp1 = &s1;

    bufferMaster.event       = &eventMaster;
    bufferMaster.data        = (uint8_t*)dataMaster;
    bufferMaster.size  	     = 128;
    bufferSlave.event        = &eventSlave;
    bufferSlave.data         = (uint8_t*)dataSlave;
    bufferSlave.size  	     = 128;
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
        	strncpy ( dataMaster, text1, 128 );
        	bufferMaster.size =  strlen(text1);
        	ssp0->transfer(&bufferMaster);
        	eventMaster.wait();

        	dataMaster[80] =10;
        	dataMaster[81] =0;
        	mutex.lock();
        	serial->writeLine("\nTask1: ");
        	serial->writeLine(dataMaster);
        	mutex.unlock();
        	LED1::Cpl();
        	sleep(100);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        while(1)
        {
        	char* text2 = "Slave: oeff I have to work so hard";
        	strncpy ( dataSlave, text2, 128 );
        	bufferSlave.size =  strlen(text1);;
        	ssp1->transfer(&bufferSlave);
        	eventSlave.wait();
        	dataSlave[80] =10;
        	dataSlave[81] =0;
        	mutex.lock();
        	serial->writeLine("\nTask2: ");
        	serial->writeLine(dataSlave);
        	mutex.unlock();
        	LED2::Cpl();
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {
        for(;;) {
        	sleep(10);
//            LED3::Cpl();
        }
    }

    template <>
    OS_PROCESS void TProc4::exec()
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




