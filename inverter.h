/*
 * inverter.h
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#ifndef INVERTER_H_
#define INVERTER_H_

#include "types.h"

//main relays
#define RELAY_PORT PORTC
#define RELAY_DDR  DDRC

#define INVERTER   PC5
#define BYPASS     PC4

//charg relays
#define CHG_RELAY_PORT PORTB
#define CHG_RELAY_DDR  DDRB

#define SUN_CHG PB0
#define AC_CHG  PB1

//internal adc
#define AC_IN     6
#define AC_OUT 	  3
#define AC_CUR    1
#define BATTARY_V 2
#define DC_CUR 	  0
#define CHG_CUR   7

//external adc
#define PV_CUR     1
#define PV_V       2
#define PV_TEMP    3
#define TRANS_TEMP 4
#define INV_TEMP   5
#define CHG_TEMP   6

#define ADC_COUNT 10

typedef enum {
	SUB,
	SBU
}_s_priority;

typedef enum{
	SNU,
	CSO,
	OSO
}_chg_priority;

typedef struct {
	_s_priority output_priority;
	unsigned char voltage_to_utility;
	unsigned char voltage_to_battary;
	_chg_priority chg_priority;

}_settings;

typedef enum{
	InputOutputAC=1,
	PvVoltage=2,
	ChgCurrent=3,
	BatVoltage=4,
	LoadPercent=5,
	LoadWats=6,
	ChargCurrentBoth=7,
	ChargPower=8

}lcd_state;

typedef enum {
	BattaryMode,
	LineMode
}inv_state;

typedef struct {
	unsigned int ac_voltage_in;
	unsigned int ac_voltage_out;
	unsigned int dc_voltage;
	unsigned int pv_voltage;

	unsigned int ac_out_current;
	unsigned int dc_current;
	unsigned int charg_current;
	unsigned int pv_current;

	unsigned char lcd_state;
	inv_state inv_state;

	unsigned char battary_percent;
	unsigned char load_percent;
	unsigned int load_wats;
	unsigned char inverter_on_off;
	unsigned char charg_on_off;
	unsigned char pv_charg_on_off;

	unsigned char ac_detect;
	unsigned char pv_detect;
	unsigned char bypass;

}inverter_state;

void set_read_adc_inverter(_read_adc read);
void inverter_init();
void inverter_process();

#endif /* INVERTER_H_ */
