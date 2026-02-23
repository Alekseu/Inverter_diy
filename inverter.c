/*
 * inverter.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#include<avr/io.h>
#include<util/delay.h>
#include <avr/eeprom.h>
#include "inverter.h"
#include "lcd.h"
#include "ext_adc.h"
#include "adc.h"
#include "timer.h"

#define MEASURMENT_DELAY 5
#define CALCULATION_DELAY 10

#define AC_IN_MIN_VOLTAGE 190

//wcs1600
const int sensitivity_dc = 22; // 22 mV/A
const char correction = 100;   //in miliamps
//acs715
const int sensitivity = 66;    // 185mV/A for 5A model, 100 for 20A, 66 for 30A
const int adcOffset = 512;

EEMEM _settings e_settings;

_settings inverter_settings;
inverter_state _state;
Keys _key;

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

//private functions;
void measurment();
void calculation();
unsigned int calcAVG(unsigned int *buff);
void main_relay_init();
void charg_relay_init();
void calc_battary_percent();

void inverter_init(){

	//чтение настроек с eeprom
	eeprom_read_block(&inverter_settings, &e_settings, sizeof(inverter_settings));

//	inverter_settings.chg_priority = SNU;
//	inverter_settings.output_priority = SUB;
//	inverter_settings.voltage_to_battary = 20;
//	inverter_settings.voltage_to_utility = 22;

	//запись настроек в eeprom
	//eeprom_write_block(&inverter_settings, &e_settings, sizeof(inverter_settings));

	_state.lcd_state = ChargCurrentBoth;
	_state.inverter_on_off = true;
	_state.pv_charg_on_off = true;
	_state.pv_detect =true;

	main_relay_init();
	charg_relay_init();
	ht1621_init();
	lcd_clear();
	lcd_buf_clear();
	drow_input(true);
	drow_output(true);
	drow_lines_under(true);
	drow_bat(0,false);
	adc_init();
	set_read_adc(MCP3008_Read);

	//external adc
	SPI_Init();
	//SPI_Init_Interrupt();
	//MCP3008_StartRead(0);
	init_wait(&_key_scan,200);
	init_wait(&_measurment,MEASURMENT_DELAY);
	init_wait(&_calculation,CALCULATION_DELAY);
}

void inverter_process(){

	if(_key_scan.isElapsed(&_key_scan)){
		_key_scan.reset(&_key_scan);
		_key = read_buttons();
		switch(_key){
		case NoKey:
			break;
		case EscKey:
			break;
		case UpKey:
			if(_state.lcd_state++>=7)_state.lcd_state=1;
			break;
		case DownKey:
			if(_state.lcd_state--<=1)_state.lcd_state=7;
			break;
		case MenuKey:
			break;
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
	drow_bat(_state.battary_percent,false);
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

		}
		break;
		case ChgCurrent:{
			drow_bat_left(true);
			drow_bat_right(true);
			drow_pv(true);
			drow_digits_left(_state.pv_current,true);
			drow_digits_right(_state.dc_current,true);
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
			drow_digits_left(_state.ac_voltage_in,false);
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
			//ac_pv_cur = _state.charg_current+_state.pv_current;

			drow_digits_left(adc_result,true);
			drow_digits_right(adc_result,true);

			drow_A_left(true);
			drow_A_right(true);
			drow_lines_under(true);
		}
		break;
		case ChargPower:{

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

	if(_state.ac_voltage_in>AC_IN_MIN_VOLTAGE){
		drow_grig(true);
		drow_lamp(true);
		RELAY_PORT|=(1<<BYPASS);
		drow_bypass(true);
		_state.bypass = true;
		_state.charg_on_off = true;
	} else {
		drow_grig(false);
		RELAY_PORT&=~(1<<BYPASS);
		drow_bypass(false);
		_state.bypass = false;
		_state.charg_on_off = false;
	}

	drow_solar(_state.pv_detect,3);

	if(_state.charg_on_off){
		CHG_RELAY_PORT|=(1<<AC_CHG);
		chg_led(true);
	}else{
		CHG_RELAY_PORT&=~(1<<AC_CHG);
		chg_led(false);
	}

	if(_state.charg_on_off || _state.pv_charg_on_off){
		drow_charger(true,true, _state.pv_charg_on_off,_state.charg_on_off,false);
	} else{
		drow_charger(false,false,false,false,false);
	}

	if(_state.pv_charg_on_off){
		CHG_RELAY_PORT|=(1<<SUN_CHG);
	} else {
		CHG_RELAY_PORT&=~(1<<SUN_CHG);
	}

}

void measurment(){

	adc_result = MCP3008_Read(0);

	Battary[buffer_counter]=read_adc(BATTARY_V);

	Ac_in[buffer_counter]= read_adc(AC_IN);

	Ac_out[buffer_counter]= read_adc(AC_OUT);

	Dc_cur[buffer_counter]= read_adc(DC_CUR);

	Chg_cur[buffer_counter] = read_adc(CHG_CUR);

	Pv_cur[buffer_counter] = read_adc(PV_CUR);

	Pv_v[buffer_counter] = MCP3008_Read(2);

	if(buffer_counter++>=ADC_COUNT-1){
		buffer_counter=0;
	}
}

void calculation(){
	unsigned long temp=0;
	float voltage=0;
	temp = calcAVG(Battary);
	_state.dc_voltage = (temp*23.7/488)*10;

	temp= calcAVG(Ac_in);
	_state.ac_voltage_in = (temp*228/470);

	temp = calcAVG(Ac_out);
	_state.ac_voltage_out = (temp*228/470);

	temp = calcAVG(Dc_cur);
	if(temp>=adcOffset)voltage = (temp - adcOffset);
	if(temp<adcOffset)voltage = (adcOffset-temp);
	voltage = voltage * (5000.0 / 1023.0);
	voltage = (voltage / sensitivity_dc)*10;
	_state.dc_current = voltage;

	temp = calcAVG(Chg_cur);
	voltage = (temp - adcOffset) * (5000.0 / 1023.0);
	_state.charg_current = (voltage / sensitivity)*10;

	temp = calcAVG(Pv_cur);
	voltage = (temp - adcOffset) * (5000.0 / 1023.0);
	_state.pv_current = (voltage / sensitivity)*10;

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

