/*
 * lcd.h
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */
#include <avr/io.h>
#include "lcd_map.h"
#include "types.h"

#ifndef LCD_H_
#define LCD_H_

#define LCD_PORT PORTD
#define LCD_DDR DDRD

#define LCD_CS   PD5
#define LCD_DATA PD7
#define LCD_WR   PD6

#define LCD_COUNT 32

//leds
#define AC_INV_LED PD2
#define CHG_LED    PD3
#define FAULT_LED  PD4

//buttons (adc pin)
#define BUTTONS      0

#define  BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
#define  SYSDIS   0X00             //0b1000 0000 0000
#define  SYSEN    0X02             //0b1000 0000 0010
#define  LCDOFF   0X04             //0b1000 0000 0100
#define  LCDON    0X06             //0b1000 0000 0110
#define  XTAL     0x28             //0b1000 0010 1000
#define  RC256    0X30             //0b1000 0011 0000
#define  TONEON   0X12             //0b1000 0001 0010
#define  TONEOFF  0X10             //0b1000 0001 0000
#define  WDTDIS1  0X0A

typedef  enum  {
	NoKey =0,
	EscKey=1,
	UpKey =2,
	DownKey=3,
	MenuKey=4
}Keys;

extern unsigned char lcd_ram[LCD_COUNT];


//низкоуровневые функции работы с экраном
void ht1621_init(void);
void set_read_adc(_read_adc read);
void lcd_clear(void);
void wrDATA(unsigned char data, unsigned char cnt);
void wrCMD(unsigned char CMD);
void wrdata(unsigned char addr, unsigned char sdata);
void wrCLR(unsigned char len);

//работа с буфером экрана
void lcd_buf_clear(void);
void lcd_set_seg(lcd_seg_t seg);
void lcd_clr_seg(lcd_seg_t seg);
void lcd_update(void);

//отризовка символов на экране
//show digits
void drow_digits_left(unsigned int num,bool dot);
void drow_digits_midle(unsigned char num);
void drow_digits_right(unsigned int num,bool dot);
void drow_bat(unsigned char level, bool charging);
void drow_invertor(bool state, bool batState,bool transit );
void drow_charger(bool state, bool batState, bool solar , bool grid, bool wind);
void drow_bypass(bool state);
void drow_solar(bool state, unsigned char num);
void drow_wind(bool state);
void drow_grig(bool state);
void drow_lamp(bool state);
void drow_load(bool state, unsigned int load, bool overload);
void drow_error(bool state);
void drow_warning(bool state);
void drow_mute(bool state);
void drow_clock(bool state);
void drow_service(bool state);
void drow_menu(bool state, unsigned char pMenu);
void drow_bat_left(bool state);
void drow_bat_right(bool state);
void drow_right_load(bool state);
void drow_temp(bool state, unsigned int temp);
void drow_input(bool state);
void drow_output(bool state);
void drow_ac(bool state);
void drow_pv(bool state);
void drow_wp(bool state);
void drow_lines_under(bool state);
void drow_inv(bool state);
void drow_transit(bool state);
void drow_eco(bool state);
void drow_v_left(bool state);
void drow_v_right(bool state);
void drow_A_left(bool state);
void drow_A_right(bool state);
void drow_right_percent(bool state);
void drow_right_wats(bool state);
void drow_right_kilo_wats(bool state);

//управление светодиодами
void ac_inv_led(bool state);
void chg_led(bool state);
void fault_led(bool state);

//чтение кнопок
Keys read_buttons();

#endif /* LCD_H_ */
