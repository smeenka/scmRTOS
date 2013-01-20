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
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}



void UART0_PrintString(SERIAL::Serial* ser,char*  s);
extern int lua_main (int argc, char **argv);

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//      Process types
typedef OS::process<OS::pr3, 1000> TProcSend;
typedef OS::process<OS::pr4, 3000> TProc3 ; //lowest
typedef OS::process<OS::pr0, 500> TProcReceive; // lowest

//---------------------------------------------------------------------------
//      Process objects
//
TProcSend ProcSend;
TProc3 Proc3;
TProcReceive ProcReceive;

//---------------------------------------------------------------------------
tick_count_t tick_count;        // global variable for OS::GetTickCount testing
int i = 0, j = 0,k = 0,l=0,m=0;

OS::TEventFlag event;


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
    UART0_PrintString(serial,"dispatcher-unittest: starting\r\n");

//    ScmPrintf::init(serial);
//    Dispatcher::init(serial);
//   Dispatcher::registerListener(0,&event);

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
             sleep(5000);
             //UART0_PrintString(serial,"Task1: Please enter some text. Tick count: %4d -\r\n");
             int32_t tick_count = OS::get_tick_count();
//             ScmPrintf::print(text, (tick_count)  );
        }
    }


	template <>
    OS_PROCESS void TProc3::exec()
    {
        LED3::On();

        // Start Lua directly
        char* lua_argv[] = { "lua", NULL };
        lua_main( 1, lua_argv );


        lua_State *L = luaL_newstate();  /* create state */
        LED3::Off();
        while(1){
            LED3::Cpl();
            sleep(500);

        }
    }

    template <>
    OS_PROCESS void TProcReceive::exec()
    {
        for(;;) {
        	event.wait();
        	char*  buffer = Dispatcher::readLine(0);
//            ScmPrintf::print( buffer);
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


void UART0_PrintString(SERIAL::Serial* ser,char*  s)
{
	// loop through until reach string's zero terminator
	int  ix = 0;
	while(s[ix] != 0){
		ser->write(s[ix]); // print each character
		ix++;
	}
}


