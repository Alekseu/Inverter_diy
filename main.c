/*
 * main.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"

wait _lcd_update;
bool state=true;

void callback_func(){
	ac_inv_led(state);
	if(state){
		state = false;
	} else {
		state = true;
	}
}
#if MCP_USE_ISR==1
void start_read_mcp3008(){
	StartRead();

}
#endif
#if USE_ISR==1
void start_read_internal(){
	StartRead_internal();
}
#endif


int main(){
	init_timers();
	init_wait(&_lcd_update,50);

#if MCP_USE_ISR==1
	setTimer(1,2,&start_read_mcp3008);
#endif
#if USE_ISR==1
	setTimer(2,1,&start_read_internal);
#endif

	inverter_init();

	while(1){

		if(_lcd_update.isElapsed(&_lcd_update)){
			_lcd_update.reset(&_lcd_update);
			lcd_update();
		}

		inverter_process();

	}
	return 0;
}

