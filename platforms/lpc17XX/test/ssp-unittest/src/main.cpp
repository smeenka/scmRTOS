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

using namespace SERIAL;

Serial* serial;
LpcSSP* ssp0;
LpcSSP* ssp1;

int main()
{
    board_setup();
    LED4::On();
    LpcSerial lpcserial(0,SERIAL::BAUD_115200);
    serial = &lpcserial;
    serial->writeLine("ssp-unit loopback test: starting\r\n");

    LpcSSP s0 = LpcSSP(0,8, LpcSSP::MASTER_LB);
    s0.speed(LpcSSP::BIT_10M);
    ssp0 = &s0;

    LpcSSP s1 = LpcSSP(1,8, LpcSSP::MASTER_LB);
    s1.speed(LpcSSP::BIT_10M);
    ssp1 = &s1;

    // run
    OS::run();
}

void pulse_train(){
	uint16_t align = 0;
	serial->writeLine("sdb: pulse train\n");

    for(int i = 0 ; i < 8 ; i++)
        ssp1->write(0xFF);
    while (ssp1->busy() ) {};
    serial->writeLine("sdb: pulse train finished\n");
}


namespace OS
{
    template <>
    OS_PROCESS void TProc1::exec()
    {
    	char task1_text[] = "Task1: in the beginning of time\r\n";
    	int len = strlen(task1_text);
        while(1)
        {
        	for(int i = 0; i < 0; i++){
        		uint16_t rec = ssp0->transfer((uint16_t) task1_text[i]);
        		serial->write((char) rec);
        	}
            sleep(10);
            LED1::Cpl();
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
    	char task2_text[] = "Task2: God created the world in 7 days\r\n";
    	int len = strlen(task2_text);
        while(1)
        {
        	for(int i = 0; i < 0; i++){
        		uint16_t rec = ssp1->transfer((uint16_t) task2_text[i]);
        		serial->write((char) rec);
        	}
        	pulse_train();
            sleep(10);
            LED2::Cpl();
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {
        for(;;) {
            sleep( 11);
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
    	LED3::Cpl();
    }

}
#endif




