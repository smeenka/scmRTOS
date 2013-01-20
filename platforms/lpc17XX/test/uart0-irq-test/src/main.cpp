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



void UART0_PrintString(SERIAL::Serial* ser,char*  s);
void callbackSend();
void callbackRcv();
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr2, 1000> TProcSend;
typedef OS::process<OS::pr1, 300> TProc2;
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


SERIAL::Serial* serial;

SERIAL::TSerialBuffer sendBuffer,rcvBuffer;

OS::TMutex mutex;
OS::TEventFlag eventSend, eventRcv;

char dataSend[128];
char dataRcv[128];

int main()
{

    board_setup();
    SERIAL::LpcSerial lpcserial(0,SERIAL::BAUD_115200);
    LED4::On();

    serial = &lpcserial;
    // run
    UART0_PrintString(serial,"serial-irq-unittest: starting\r\n");

    sendBuffer.event         = &eventSend;
    sendBuffer.data          = (uint8_t*) dataSend;
    sendBuffer.channel       = 0;
    sendBuffer.size  	     = 128;
	sendBuffer.stopchar 	 = 10;

    rcvBuffer.event          =  &eventRcv;
    rcvBuffer.data			 = (uint8_t*)dataRcv;
    rcvBuffer.size		     = 128;
    rcvBuffer.channel	     = 0;
    rcvBuffer.stopchar 	 	= 10;
    serial->enableInterruptTx();
    serial->enableInterruptRx();
    LED1::On();
    LED2::On();

    OS::run();
}


namespace OS
{
    template <>
    OS_PROCESS void TProcSend::exec()
    {
        sleep(1);
        while(1)
        {
            LED1::Cpl();

        	mutex.lock();
        	strncpy ( dataSend, "Task1: in the beginning of time\r\n", 128 );
        	serial->transmit(&sendBuffer);
        	eventSend.wait();

        	strncpy ( dataSend, "Task1: God created the world\r\n", 128 );
        	serial->transmit(&sendBuffer);
        	eventSend.wait();
        	mutex.unlock();
             sleep(1000);
        }
    }

    template <>
    OS_PROCESS void TProc2::exec()
    {
        sleep(1);
        for(;;) {
            LED2::Cpl();
        	mutex.lock();
        	strcpy ( dataSend, "Task2: did he do this job in 7 days?\r\n" );
        	serial->transmit(&sendBuffer);
        	eventSend.wait();
        	mutex.unlock();
            sleep(5000);
        }
    }

    template <>
    OS_PROCESS void TProc3::exec()
    {
        for(;;) {
            tick_count = OS::get_tick_count();

//            UART0_PrintString(serial,"z");
            sleep( 100);
        }
    }

    template <>
    OS_PROCESS void TProcReceive::exec()
    {
        for(;;) {
        	serial->receive(&rcvBuffer);
        	eventRcv.wait();
            LED3::Cpl();
        	mutex.lock();
        	int n = strlen(dataRcv);
        	if (n < 128)
        		dataRcv[n]= 10;				//replace end of string with end of line character
        	dataRcv[127] = 10;

          	strncpy ( dataSend, dataRcv,128 );
        	serial->transmit(&sendBuffer);
        	eventSend.wait();
        	mutex.unlock();
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


void UART0_PrintString(SERIAL::Serial* ser,char*  s)
{
	// loop through until reach string's zero terminator
	int  ix = 0;
	while(s[ix] != 0){
		ser->write(s[ix]); // print each character
		ix++;
	}
}


