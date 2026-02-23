/*
 * main.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"

unsigned char lcd_update_counter=0;
wait _led;
wait _lcd_update;
unsigned char t=1;


int main(){
	init_timers();
	init_wait(&_led,500);
	init_wait(&_lcd_update,100);
	inverter_init();


	while(1){

		if(_lcd_update.isElapsed(&_lcd_update)){
			_lcd_update.reset(&_lcd_update);
			lcd_update();
		}

		inverter_process();

		if(_led.isElapsed(&_led)){
			_led.reset(&_led);
			chg_led(t);
			if(t){
				t=0;
			} else {
				t=1;
			}
		}
	}
}

