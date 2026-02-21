/*
 * lcd.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#include "lcd.h"

unsigned char lcd_ram[LCD_COUNT]={0};

_read_adc read_adc_lcd;

// Маска сегментов: A B C D E F G (7 бит)
// Порядок бит в маске: [0][G][F][E][D][C][B][A]
const unsigned char segment_map[] = {
    0x3F, // 0: 0b00111111
    0x06, // 1: 0b00000110
    0x5B, // 2: 0b01011011
    0x4F, // 3: 0b01001111
    0x66, // 4: 0b01100110
    0x6D, // 5: 0b01101101
    0x7D, // 6: 0b01111101
    0x07, // 7: 0b00000111
    0x7F, // 8: 0b01111111
    0x6F  // 9: 0b01101111
};

//низкоуровневые функции работы с экраном
void ht1621_init(void)
{
	LCD_DDR|=(1<<LCD_CS)|(1<<LCD_WR)|(1<<LCD_DATA)|(1<<AC_INV_LED)|(1<<CHG_LED)|(1<<FAULT_LED);
	LCD_PORT|=(1<<LCD_CS);
	LCD_PORT|=(1<<LCD_WR);

	_delay_ms(10);
		wrCMD(BIAS);
		wrCMD(RC256);
		wrCMD(SYSDIS);
		wrCMD(WDTDIS1);
		wrCMD(SYSEN);
		wrCMD(LCDON);
}

void set_read_adc(_read_adc read){
	read_adc_lcd = read;
}

void lcd_clear(void)
{
	wrCLR(LCD_COUNT);
}

void wrDATA(unsigned char data, unsigned char cnt) {
	unsigned char i;
	for (i = 0; i < cnt; i++) {
		LCD_PORT&=~(1<<LCD_WR);
		_delay_us(5);
		if (data & 0x80) {
			LCD_PORT|=(1<<LCD_DATA);
		}
		else {
			LCD_PORT&=~(1<<LCD_DATA);
		}
		LCD_PORT|=(1<<LCD_WR);
		_delay_us(5);
		data <<= 1;
	}
}

void wrCMD(unsigned char CMD) {  //100
	LCD_PORT&=~(1<<LCD_CS);
	wrDATA(0x80, 4);
	wrDATA(CMD, 8);
	LCD_PORT|=(1<<LCD_CS);
}

void wrdata(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	LCD_PORT&=~(1<<LCD_CS);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	sdata<<=4;
	wrDATA(sdata, 4);
	LCD_PORT|=(1<<LCD_CS);
}

void wrCLR(unsigned char len) {
	unsigned char i;
	for (i = 0; i < len; i++) {
		wrdata(i, 0x00);
	}
}

//работа с буфером экрана
void lcd_buf_clear(void){
	for(unsigned char i=0;i<LCD_COUNT;i++){
		lcd_ram[i]=0x00;
	}
}

void lcd_set_seg(lcd_seg_t seg){
	lcd_ram[seg.addr]|= (1<<seg.bit);
}

void lcd_clr_seg(lcd_seg_t seg){
	lcd_ram[seg.addr]&=~(1<<seg.bit);
}

void lcd_update(void){
	for(unsigned char i=0;i<LCD_COUNT;i++){
		wrdata(i, lcd_ram[i]);
	}
}

//отрисовка символов экрана
void drow_bat(unsigned char level,bool charging){
	lcd_set_seg(S46);
	lcd_clr_seg(S42);
	lcd_clr_seg(S43);
	lcd_clr_seg(S44);
	lcd_clr_seg(S45);

	if(level>5 && level<=25){
		lcd_set_seg(S42);
	}
	if(level>25 && level<=50){
		lcd_set_seg(S42);
		lcd_set_seg(S43);
	}
	if(level>50 && level<=75){
		lcd_set_seg(S42);
		lcd_set_seg(S43);
		lcd_set_seg(S44);
	}
	if(level>75 && level<=100){
		lcd_set_seg(S42);
		lcd_set_seg(S43);
		lcd_set_seg(S44);
		lcd_set_seg(S45);
	}
	if(charging) lcd_set_seg(S57); else lcd_clr_seg(S57);
}

void drow_invertor(bool state,bool batState, bool transit){
	if(state){
		if(batState)lcd_set_seg(S48);
		lcd_set_seg(S49);
		lcd_set_seg(S52);
		lcd_set_seg(S54);
	} else {
		lcd_clr_seg(S48);
		lcd_clr_seg(S49);
		lcd_clr_seg(S52);
		lcd_clr_seg(S54);
	}
	if(transit) lcd_set_seg(S47); else lcd_clr_seg(S47);
}

void drow_charger(bool state,bool batState,bool solar, bool grid, bool wind){
	if(state){
			lcd_set_seg(S38);
			if(batState)lcd_set_seg(S41);

		} else {
			lcd_clr_seg(S38);
			lcd_clr_seg(S41);

		}
	if(wind)lcd_set_seg(S35); else lcd_clr_seg(S35);
	if(grid)lcd_set_seg(S37); else lcd_clr_seg(S37);
	if(solar)lcd_set_seg(S39); else lcd_clr_seg(S39);

}

void drow_bypass(bool state){
	if(state){
		lcd_set_seg(S36);
		lcd_set_seg(S62);
	} else {
		lcd_clr_seg(S36);
		lcd_clr_seg(S62);
	}
}

void drow_solar(bool state, unsigned char num){
	if(state){
		lcd_set_seg(S64);
		if(num==0) {lcd_clr_seg(S65);lcd_clr_seg(S66);lcd_clr_seg(S67);}
		if(num==1) {lcd_set_seg(S67);}
		if(num==2) {lcd_set_seg(S66);lcd_set_seg(S67);}
		if(num==3) {lcd_set_seg(S65);lcd_set_seg(S66);lcd_set_seg(S67);}
	} else {
		lcd_clr_seg(S64);
		lcd_clr_seg(S65);
		lcd_clr_seg(S66);
		lcd_clr_seg(S67);
	}
}

void drow_wind(bool state){
	if(state)lcd_set_seg(S33); else lcd_clr_seg(S33);
}

void drow_grig(bool state){
	if(state)lcd_set_seg(S34); else lcd_clr_seg(S34);
}

void drow_lamp(bool state){
	if(state){
		lcd_set_seg(S54);
	} else {
		lcd_clr_seg(S54);
	}
}

void drow_error(bool state){
	if(state){
		lcd_set_seg(S21);
		lcd_set_seg(S22);
	}else {
		lcd_clr_seg(S21);
		lcd_clr_seg(S22);
	}
}

void drow_warning(bool state){
	if(state){
			lcd_set_seg(S21);
		}else {
			lcd_clr_seg(S21);
		}
}

void drow_mute(bool state){
	if(state){
		lcd_set_seg(S63);
	} else {
		lcd_clr_seg(S63);
	}
}

void drow_clock(bool state){
	if(state){
		lcd_set_seg(S19);
	} else {
		lcd_clr_seg(S19);
	}
}

void drow_service(bool state){
	if(state){
		lcd_set_seg(S20);
	} else {
		lcd_clr_seg(S20);
	}
}

void drow_menu(bool state, unsigned char pMenu){
	if(state){
		lcd_set_seg(S18);
		lcd_set_seg(S32);
		drow_digits_midle(pMenu);
	}else {
		lcd_clr_seg(S18);
		lcd_clr_seg(S32);
	}
}

void drow_bat_left(bool state)
{
	if(state){
		lcd_set_seg(S6);
	} else {
		lcd_clr_seg(S6);
	}
}

void drow_bat_right(bool state){
	if(state){
		lcd_set_seg(S24);
	} else {
		lcd_clr_seg(S24);
	}
}

void drow_right_load(bool state){
	if(state){
		lcd_set_seg(S25);
	} else {
		lcd_clr_seg(S25);
	}
}

void drow_temp(bool state, unsigned int temp){
	if(state){
		lcd_set_seg(S7);
		drow_digits_left(temp,true);
		lcd_set_seg(S14);
		lcd_set_seg(S15);
	} else {
		lcd_clr_seg(S7);
		lcd_clr_seg(S14);
		lcd_clr_seg(S15);
	}
}

void drow_load(bool state, unsigned int load, bool overload){
	if(state){
		lcd_set_seg(S56);
		if(load<5) {lcd_clr_seg(S58);lcd_clr_seg(S59);lcd_clr_seg(S60);lcd_clr_seg(S61);}
		if(load>5&&load<=25) {lcd_set_seg(S61);}
		if(load>25&&load<=50) {lcd_set_seg(S61);lcd_set_seg(S60);}
		if(load>50&&load<=75) {lcd_set_seg(S61);lcd_set_seg(S60);lcd_set_seg(S59);}
		if(load>75&&load<=100) {lcd_set_seg(S61);lcd_set_seg(S60);lcd_set_seg(S59);lcd_set_seg(S58);}
	} else {
		lcd_clr_seg(S56); //ramka
		lcd_clr_seg(S58);
		lcd_clr_seg(S59);
		lcd_clr_seg(S60);
		lcd_clr_seg(S61);
	}
	if(overload){
		lcd_set_seg(S55);
	} else {
		lcd_clr_seg(S55);
	}
}

void drow_input(bool state){
	if(state){
		lcd_set_seg(S5);
	} else {
		lcd_clr_seg(S5);
	}
}

void drow_output(bool state){
	if(state){
		lcd_set_seg(S23);
	} else {
		lcd_clr_seg(S23);
	}
}

void drow_ac(bool state){
	if(state){
		lcd_set_seg(S2);
	} else {
		lcd_clr_seg(S2);
	}
}

void drow_pv(bool state){
	if(state){
		lcd_set_seg(S3);
	} else {
		lcd_clr_seg(S3);
	}
}

void drow_wp(bool state){
	if(state){
		lcd_set_seg(S1);
	} else {
		lcd_clr_seg(S1);
	}
}

void drow_inv(bool state){
	if(state){
		lcd_set_seg(S4);
	} else {
		lcd_clr_seg(S4);
	}
}

void drow_transit(bool state){
	if(state){
		lcd_set_seg(S47);
	} else {
		lcd_clr_seg(S47);
	}
}

void drow_eco(bool state){
	if(state){
			lcd_set_seg(S40);
		} else {
			lcd_clr_seg(S40);
		}
}

void drow_lines_under(bool state){
	if(state){
		lcd_set_seg(S32);
	} else {
		lcd_clr_seg(S32);
	}
}

void ac_inv_led(bool state){
	if(state){
		LCD_PORT|=(1<<AC_INV_LED);
	} else {
		LCD_PORT&=~(1<<AC_INV_LED);
	}
}

void chg_led(bool state){
	if(state){
		LCD_PORT|=(1<<CHG_LED);
	} else {
		LCD_PORT&=~(1<<CHG_LED);
	}
}

void fault_led(bool state){
	if(state){
		LCD_PORT|=(1<<FAULT_LED);
	} else {
		LCD_PORT&=~(1<<FAULT_LED);
	}
}

void drow_v_left(bool state){
	if(state){
		lcd_set_seg(S9);
	} else {
		lcd_clr_seg(S9);
	}
}

void drow_v_right(bool state){
	if(state){
		lcd_set_seg(S27);
	} else {
		lcd_clr_seg(S27);
	}
}

void drow_A_left(bool state){
	if(state){
		lcd_set_seg(S13);
	} else {
		lcd_clr_seg(S13);
	}
}

void drow_A_right(bool state){
	if(state){
		lcd_set_seg(S31);
	} else {
		lcd_clr_seg(S31);
	}
}

void drow_right_percent(bool state){
	if(state){
			lcd_set_seg(S28);
		} else {
			lcd_clr_seg(S28);
		}
}

void drow_right_wats(bool state){
	if(state){
			lcd_set_seg(S30);
		} else {
			lcd_clr_seg(S30);
		}
}

void drow_right_kilo_wats(bool state){
	if(state){
			lcd_set_seg(S26);
			lcd_set_seg(S30);
		} else {
			lcd_clr_seg(S26);
			lcd_clr_seg(S30);
		}
}

// Универсальная функция отрисовки одной цифры
void draw_single_digit(unsigned char val, lcd_seg_t a, lcd_seg_t b, lcd_seg_t c,
										  lcd_seg_t d, lcd_seg_t e, lcd_seg_t f, lcd_seg_t g) {
    unsigned char mask = segment_map[val % 10];
    (mask & 0x01) ? lcd_set_seg(a) : lcd_clr_seg(a);
    (mask & 0x02) ? lcd_set_seg(b) : lcd_clr_seg(b);
    (mask & 0x04) ? lcd_set_seg(c) : lcd_clr_seg(c);
    (mask & 0x08) ? lcd_set_seg(d) : lcd_clr_seg(d);
    (mask & 0x10) ? lcd_set_seg(e) : lcd_clr_seg(e);
    (mask & 0x20) ? lcd_set_seg(f) : lcd_clr_seg(f);
    (mask & 0x40) ? lcd_set_seg(g) : lcd_clr_seg(g);
}

void drow_digits_left(unsigned int num, bool dot){
	draw_single_digit(num / 100, A11, B11, C11, D11, E11, F11, G11);
	draw_single_digit(num / 10,  A12, B12, C12, D12, E12, F12, G12);
	draw_single_digit(num,       A13, B13, C13, D13, E13, F13, G13);
	if(dot){
		lcd_set_seg(D2);
	} else {
		lcd_clr_seg(D2);
	}
}

void drow_digits_midle(unsigned char num){
	draw_single_digit(num / 10, A14, B14, C14, D14, E14, F14, G14);
	draw_single_digit(num,      A15, B15, C15, D15, E15, F15, G15);
}

void drow_digits_right(unsigned int num, bool dot){
	draw_single_digit(num / 100, A16, B16, C16, D16, E16, F16, G16);
	draw_single_digit(num / 10,  A17, B17, C17, D17, E17, F17, G17);
	draw_single_digit(num,       A18, B18, C18, D18, E18, F18, G18);
	if(dot){
		lcd_set_seg(D5);
	} else {
		lcd_clr_seg(D5);
	}
}

//чтение кнопок
Keys read_buttons(){
	Keys key=NoKey;
	if(read_adc_lcd!=0){
		int value = read_adc_lcd(BUTTONS);
		if(value>=840){
			key= MenuKey;
		}
		if(value>600 && value<800){
			key= DownKey;
		}
		if(value>378 && value<500){
			key= UpKey;
		}
		if(value>215 && value<350){
			key= EscKey;
		}
	}
	return key;
}
