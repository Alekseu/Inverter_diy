/*
 * inverter.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#include<avr/io.h>
#include<util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "inverter.h"
#include "lcd.h"
#include "ext_adc.h"
#include "adc.h"
#include "timer.h"
#include "term_table.h"

#define MEASURMENT_DELAY 4
#define CALCULATION_DELAY 6
#define KEY_SCAN_DELAY 21

#define AC_IN_MIN_VOLTAGE 190
#define PV_MIN_VOLTAGE 185 //25.2v *10

//wcs1600  //22mv/A
#define WCS_DIV 45 //0.02/5*1023 or (4.5 for 0.022) *10 =45
unsigned int adc_zero =0;
//acs715  // 66mv/A
#define ACS_DIV 135 //0.066/5*1023 or (13.5 for 0.066) *10 = 135
unsigned int adc_zero_acs_pv =0;
unsigned int adc_zero_acs_chg =0;
//zmct103
unsigned int adc_zero_zmct =0;

EEMEM _settings e_settings;

_settings inverter_settings;
inverter_state _state;
Keys _key;
Keys _old_key;

wait _key_scan;
wait _measurment;
wait _calculation;

unsigned int battary_voltage_source=0;
unsigned int ac_pv_cur =0;
unsigned char buffer_counter=0;

unsigned int Battary[ADC_COUNT];
unsigned int Ac_in[ADC_COUNT];
unsigned int Ac_out[ADC_COUNT];
unsigned int Ac_cur[ADC_COUNT];
unsigned int Dc_cur[ADC_COUNT];
unsigned int Pv_cur[ADC_COUNT];
unsigned int Pv_v[ADC_COUNT];
unsigned int Chg_cur[ADC_COUNT];

extern const unsigned int adc_table[111];
unsigned int CHG_Temp=0;
unsigned int INV_Temp=0;
unsigned int PV_Temp=0;
unsigned int WP_Temp=0;

//private functions;
void measurment();
void calculation();
unsigned int calcAVG(unsigned int *buff);
void main_relay_init();
void charg_relay_init();
void calc_battary_percent();
unsigned char get_temp_fast(unsigned int adc_val);
void make_adc_zero();
unsigned int read_current_A(unsigned int adc,unsigned int _adc_zero, unsigned char divider );
unsigned int get_current_ac(unsigned int _adc_zero);

void inverter_init(){

	//чтение настроек с eeprom
	eeprom_read_block(&inverter_settings, &e_settings, sizeof(inverter_settings));

//	inverter_settings.chg_priority = SNU;
//	inverter_settings.output_priority = SUB;
//	inverter_settings.voltage_to_battary = 20;
//	inverter_settings.voltage_to_utility = 22;

	//запись настроек в eeprom
	//eeprom_write_block(&inverter_settings, &e_settings, sizeof(inverter_settings));

	_state.lcd_state = LoadWats;
	_state.inverter_on_off = true;

	main_relay_init();
	charg_relay_init();
	ht1621_init();
	lcd_clear();
	lcd_buf_clear();
	drow_input(true);
	drow_output(true);
	drow_lines_under(true);
	drow_bat(0,false);

	//internal adc
	adc_init();
	//external adc
	Ext_adc_init();
	set_read_adc(MCP3008_Read); //установка метода чтени кнопок дл экрана

	init_wait(&_key_scan,KEY_SCAN_DELAY);
	init_wait(&_measurment,MEASURMENT_DELAY);
	init_wait(&_calculation,CALCULATION_DELAY);

	make_adc_zero();
}

void inverter_process(){

	if(_key_scan.isElapsed(&_key_scan)){
		_key_scan.reset(&_key_scan);
		_key = read_buttons();
		if(_key!=_old_key){
			_old_key = _key;
			switch(_key){
			case NoKey:
				break;
			case EscKey:
				break;
			case UpKey:
				if(_state.lcd_state++>=12)_state.lcd_state=1;
				break;
			case DownKey:
				if(_state.lcd_state--<=1)_state.lcd_state=12;
				break;
			case MenuKey:
				break;
			}
		}
	}

	if(_measurment.isElapsed(&_measurment)){
		_measurment.reset(&_measurment);
		measurment();
	}
	if(_calculation.isElapsed(&_calculation)){
		_calculation.reset(&_calculation);
		calculation();
	}

	lcd_buf_clear();
	drow_lamp(true);

	drow_bat(_state.battary_percent,_state.charg_on_off);
	drow_load(true,_state.load_percent,false);

	drow_digits_midle(_state.lcd_state);

	switch(_state.lcd_state){
		case InputOutputAC:{
			drow_ac(true);
			drow_input(true);
			drow_output(true);
			drow_lines_under(true);
			drow_digits_left(_state.ac_voltage_in,false);
			drow_digits_right(_state.ac_voltage_out,false);
			drow_v_left(true);
			drow_v_right(true);
			drow_lines_under(true);
		}
		break;

		case PvVoltage:{
			drow_pv(true);
			drow_input(true);
			drow_output(true);
			drow_lines_under(true);
			drow_digits_left(_state.pv_voltage, true);
			drow_digits_right(_state.ac_voltage_out, false);
			drow_v_left(true);
			drow_v_right(true);
			drow_lines_under(true);
		}
		break;

		case ChgCurrent:{
			drow_bat_left(true);
			drow_bat_right(true);
			drow_pv(true);
			drow_digits_left(_state.pv_current,true);
			drow_digits_right(_state.charg_current,true);
			drow_A_left(true);
			drow_A_right(true);
			drow_lines_under(true);
		}
		break;

		case BatVoltage:{
			drow_bat_left(true);
			drow_bat_right(true);
			drow_digits_left(_state.dc_voltage,true);
			drow_digits_right(_state.dc_current,true);
			drow_v_left(true);
			drow_A_right(true);
			drow_lines_under(true);
		}
		break;

		case LoadPercent:{
			drow_bat_left(true);
			drow_output(true);
			drow_digits_left(_state.dc_voltage,true);
			drow_v_left(true);
			drow_digits_right(_state.load_percent,false);
			drow_right_percent(true);
			drow_lines_under(true);
		}
		break;

		case LoadWats:{
			drow_input(true);
			drow_right_load(true);
			drow_digits_left(_state.ac_out_current,false);
			drow_v_left(true);
			drow_digits_right(_state.load_wats,false);
			drow_right_wats(true);
			drow_lines_under(true);
		}
		break;

		case ChargCurrentBoth:{
			drow_bat_left(true);
			drow_bat_right(true);
			drow_ac(true);
			drow_pv(true);

			drow_digits_left(ac_pv_cur,true);
			drow_digits_right(_state.dc_current,true);

			drow_A_left(true);
			drow_A_right(true);
			drow_lines_under(true);
		}
		break;

		case ChargPower:{
			drow_lines_under(true);
		}
		break;

		case Temp_CHG :{
			drow_temp(true, CHG_Temp);
			drow_ac(true);
			drow_lines_under(true);
		}
		break;
		case Temp_PV:{
			drow_temp(true, PV_Temp);
			drow_pv(true);
			drow_lines_under(true);
		}
		break;
		case Temp_INV:{
			drow_temp(true, INV_Temp);
			drow_inv(true);
			drow_lines_under(true);
		}
		break;
		case Temp_WP:{
			drow_temp(true, WP_Temp);
			drow_wp(true);
			drow_lines_under(true);
		}
		break;
		default:{

		}
		break;
	}

	drow_invertor(_state.inverter_on_off,true,false);
	if(_state.inverter_on_off){
		RELAY_PORT|=(1<<INVERTER);
		ac_inv_led(true);
	} else {
		RELAY_PORT&=~(1<<INVERTER);
		ac_inv_led(false);
	}

	drow_grig(_state.ac_detect);
	if(_state.ac_detect){
		drow_lamp(true);
		RELAY_PORT|=(1<<BYPASS);
		drow_bypass(true);
		_state.bypass = true;
		_state.charg_on_off = true;
	} else {
		RELAY_PORT&=~(1<<BYPASS);
		drow_bypass(false);
		_state.bypass = false;
		_state.charg_on_off = false;
	}

	if(_state.charg_on_off){
		CHG_RELAY_PORT|=(1<<AC_CHG);
		chg_led(true);
	}else{
		CHG_RELAY_PORT&=~(1<<AC_CHG);
		chg_led(false);
	}

	drow_solar(_state.pv_detect,3);
	if(_state.pv_charg_on_off){
		CHG_RELAY_PORT|=(1<<SUN_CHG);
		chg_led(true);
	} else {
		CHG_RELAY_PORT&=~(1<<SUN_CHG);
		chg_led(false);
	}

	if(_state.charg_on_off || _state.pv_charg_on_off){
		drow_charger(true,true, _state.pv_charg_on_off,_state.charg_on_off,false);
	} else{
		drow_charger(false,false,false,false,false);
	}



}

void measurment(){

	Battary[buffer_counter]=read_adc(BATTARY_V);

	Ac_in[buffer_counter]= read_adc(AC_IN);

	Ac_out[buffer_counter]= read_adc(AC_OUT);

	Dc_cur[buffer_counter]= read_adc(DC_CUR);

	Ac_cur[buffer_counter] = read_adc(AC_CUR);

	Chg_cur[buffer_counter] = read_adc(CHG_CUR);

	Pv_cur[buffer_counter] = MCP3008_Read(PV_CUR);

	Pv_v[buffer_counter] = MCP3008_Read(PV_V);

	if(buffer_counter++>=ADC_COUNT-1){
		buffer_counter=0;
	}

	//read temperatures 5.1k =5v termistor 10k to gnd
	 CHG_Temp=get_temp_fast(MCP3008_Read(CHG_TEMP));
	 INV_Temp=get_temp_fast(MCP3008_Read(INV_TEMP));
	 PV_Temp=get_temp_fast(MCP3008_Read(PV_TEMP));
	 WP_Temp=get_temp_fast(MCP3008_Read(TRANS_TEMP));
}

void calculation(){
	unsigned long temp=0;

	temp = calcAVG(Battary);
	_state.dc_voltage = (temp*23.7/488)*10;

	temp= calcAVG(Ac_in);
	_state.ac_voltage_in = (temp*228/470);
	if(_state.ac_voltage_in>AC_IN_MIN_VOLTAGE){
		_state.ac_detect =true;
	}else{
		_state.ac_detect = false;
	}

	temp = calcAVG(Ac_out);
	_state.ac_voltage_out = (temp*228/470);

	temp = calcAVG(Dc_cur);
	_state.dc_current = read_current_A(temp,adc_zero, WCS_DIV);

	temp = calcAVG(Chg_cur);
	_state.charg_current = read_current_A(temp, adc_zero_acs_chg,ACS_DIV);

	temp = calcAVG(Pv_cur);
	_state.pv_current = read_current_A(temp, adc_zero_acs_pv,ACS_DIV);

	_state.ac_out_current = get_current_ac(adc_zero_zmct);
	_state.load_wats = _state.ac_out_current*_state.ac_voltage_out;

	ac_pv_cur = _state.charg_current+_state.pv_current;

	temp = calcAVG(Pv_v);
	_state.pv_voltage = (temp*42.4/869)*10;
	if( _state.pv_voltage>=PV_MIN_VOLTAGE || _state.pv_voltage>=_state.dc_voltage)_state.pv_detect = true;
		else _state.pv_detect = false;

	if( _state.pv_voltage>=PV_MIN_VOLTAGE || _state.pv_voltage>=_state.dc_voltage)_state.pv_charg_on_off = true;
		else _state.pv_charg_on_off = false;

	calc_battary_percent();
}

unsigned int calcAVG(unsigned int *buff){
	unsigned long result =0;
	for(unsigned char i=0;i<ADC_COUNT;i++){
		result = result+buff[i];
	}
	return  result/ADC_COUNT;

}

void main_relay_init(){
	RELAY_DDR|=(1<<INVERTER)|(1<<BYPASS);
}

void charg_relay_init(){
	CHG_RELAY_DDR|=(1<<SUN_CHG)|(1<<AC_CHG);
}

void calc_battary_percent(){
	if(_state.dc_voltage>249){
		_state.battary_percent=100;
	}
	if(_state.dc_voltage>234&& _state.dc_voltage<249){
		_state.battary_percent=75;
	}
	if(_state.dc_voltage>222&& _state.dc_voltage<233){
		_state.battary_percent=50;
	}
	if(_state.dc_voltage>215&& _state.dc_voltage<221){
		_state.battary_percent=25;
	}
	if(_state.dc_voltage>190&& _state.dc_voltage<214){
		_state.battary_percent=10;
	}
	if(_state.dc_voltage>180&& _state.dc_voltage<188){
		_state.battary_percent=2;
	}
}

unsigned int read_current_A(unsigned int adc,unsigned int _adc_zero, unsigned char divider ){
	unsigned int delta =0;
	if(adc > _adc_zero) delta = adc-_adc_zero;
	if(_adc_zero > adc) delta = _adc_zero- adc;

	unsigned long temp = delta*100;

	return temp/divider;
}

void make_adc_zero(){
	long sum =0;
	long sum_acs_pv =0;
	long sum_acs_chg =0;
	long sum_zmct =0;
	for(unsigned char i=0;i<200;i++){
		sum+= read_adc(DC_CUR);
		sum_acs_pv+= MCP3008_Read(PV_CUR);
		sum_acs_chg+= read_adc(CHG_CUR);
		sum_zmct+= read_adc(AC_CUR);
		_delay_ms(5);
	}
	//
	adc_zero = sum/200;
	adc_zero_acs_pv = sum_acs_pv/200;
	adc_zero_acs_chg = sum_acs_chg/200;
	adc_zero_zmct = sum_zmct/200;
}

unsigned char get_temp_fast(unsigned int adc_val) {
    // Выход за границы диапазона
    if (adc_val > pgm_read_word(&adc_table[0])) return 10;   // Ниже 10°C
    if (adc_val < pgm_read_word(&adc_table[110])) return 120; // Выше 120°C

    // Бинарный поиск или обычный перебор (для 110 элементов перебор сойдет)
    for (unsigned char i = 0; i < 110; i++) {
    	unsigned int val1 = pgm_read_word(&adc_table[i]);
    	unsigned int val2 = pgm_read_word(&adc_table[i + 1]);

        if (adc_val <= val1 && adc_val >= val2) {
            // Мы нашли нужный градус i + 10
            return i + 10;
        }
    }
    return 0;
}

unsigned int isqrt32(unsigned long x)
{
	unsigned long op  = x;
	unsigned long res = 0;
	unsigned long one = 1UL << 30;

    while (one > op)
        one >>= 2;

    while (one != 0)
    {
        if (op >= res + one)
        {
            op -= res + one;
            res = res + 2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return (unsigned int)res;
}

unsigned int get_current_ac(unsigned int _adc_zero){
	unsigned long sum =0;
	for(unsigned int i=0;i<ADC_COUNT;i++)
	{
		unsigned int v =0;
		if(_adc_zero > Ac_cur[i]) v = _adc_zero-Ac_cur[i];
		if(Ac_cur[i]>_adc_zero) v = Ac_cur[i] - _adc_zero;

		sum += (unsigned long)v * v;
	}
	unsigned long mean = sum / ADC_COUNT;
	unsigned int rms_adc = isqrt32(mean);

	// коэффициент калибровки!
	float current = rms_adc * 0.050;

	return current*10;

}
