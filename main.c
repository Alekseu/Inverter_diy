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


int main(){
	init_timers();
	init_wait(&_lcd_update,100);
	inverter_init();
	setTimer(1,1000,&callback_func);

	while(1){

		if(_lcd_update.isElapsed(&_lcd_update)){
			_lcd_update.reset(&_lcd_update);
			lcd_update();
		}

		//inverter_process();

	}
}

