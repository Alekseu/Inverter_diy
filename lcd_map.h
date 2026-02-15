/*
 * lcd_map.h
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#ifndef LCD_MAP_H_
#define LCD_MAP_H_

typedef struct{
	unsigned char addr;
	unsigned char bit;
}lcd_seg_t;

#define S1 (lcd_seg_t){0,3} //wp
#define S2 (lcd_seg_t){0,2} //ac
#define S3 (lcd_seg_t){0,1} //pv
#define S4 (lcd_seg_t){0,0} //inv
#define S5 (lcd_seg_t){1,3} //input
#define S6 (lcd_seg_t){1,2} //batt
#define S7 (lcd_seg_t){1,1} //temp
#define S8 (lcd_seg_t){1,0} //k

#define D11 (lcd_seg_t){2,0}
#define E11 (lcd_seg_t){2,1}
#define G11 (lcd_seg_t){2,2}
#define F11 (lcd_seg_t){2,3}
#define C11 (lcd_seg_t){3,1}
#define B11 (lcd_seg_t){3,2}
#define A11 (lcd_seg_t){3,3}
#define D1  (lcd_seg_t){3,0} //dot

#define D12 (lcd_seg_t){4,0}
#define E12 (lcd_seg_t){4,1}
#define G12 (lcd_seg_t){4,2}
#define F12 (lcd_seg_t){4,3}
#define C12 (lcd_seg_t){5,1}
#define B12 (lcd_seg_t){5,2}
#define A12 (lcd_seg_t){5,3}
#define D2  (lcd_seg_t){5,0} //dot

#define D13 (lcd_seg_t){6,0}
#define E13 (lcd_seg_t){6,1}
#define G13 (lcd_seg_t){6,2}
#define F13 (lcd_seg_t){6,3}
#define C13 (lcd_seg_t){7,1}
#define B13 (lcd_seg_t){7,2}
#define A13 (lcd_seg_t){7,3}

#define HZ1 (lcd_seg_t){7,0}
#define S12 (lcd_seg_t){8,3} //W1
#define S13 (lcd_seg_t){8,2} //A1
#define S9 (lcd_seg_t) {8,1} //V1
#define S10 (lcd_seg_t){8,0}//half %

#define S14 (lcd_seg_t){9,1} //2 half %
#define S15 (lcd_seg_t){9,0} //C1
#define S17 (lcd_seg_t){9,2} //M1
#define S16 (lcd_seg_t){9,3} //H1

#define D14 (lcd_seg_t){10,0}
#define E14 (lcd_seg_t){10,1}
#define G14 (lcd_seg_t){10,2}
#define F14 (lcd_seg_t){10,3}
#define C14 (lcd_seg_t){11,1}
#define B14 (lcd_seg_t){11,2}
#define A14 (lcd_seg_t){11,3}
#define D3  (lcd_seg_t){11,0} //dot

#define D15 (lcd_seg_t){12,0}
#define E15 (lcd_seg_t){12,1}
#define G15 (lcd_seg_t){12,2}
#define F15 (lcd_seg_t){12,3}
#define C15 (lcd_seg_t){13,1}
#define B15 (lcd_seg_t){13,2}
#define A15 (lcd_seg_t){13,3}

#define S18 (lcd_seg_t){13,0} //ramka
#define S19 (lcd_seg_t){14,0} //clock
#define S20 (lcd_seg_t){14,1} //service key
#define S21 (lcd_seg_t){14,2} //ahtung
#define S22 (lcd_seg_t){14,3} //error

#define S26 (lcd_seg_t){15,0}
#define S25 (lcd_seg_t){15,1}
#define S24 (lcd_seg_t){15,2}
#define S23 (lcd_seg_t){15,3} //output

#define D16 (lcd_seg_t){16,0}
#define E16 (lcd_seg_t){16,1}
#define G16 (lcd_seg_t){16,2}
#define F16 (lcd_seg_t){16,3}
#define C16 (lcd_seg_t){17,1}
#define B16 (lcd_seg_t){17,2}
#define A16 (lcd_seg_t){17,3}
#define D4  (lcd_seg_t){17,0} //dot

#define D17 (lcd_seg_t){18,0}
#define E17 (lcd_seg_t){18,1}
#define G17 (lcd_seg_t){18,2}
#define F17 (lcd_seg_t){18,3}
#define C17 (lcd_seg_t){19,1}
#define B17 (lcd_seg_t){19,2}
#define A17 (lcd_seg_t){19,3}
#define D5  (lcd_seg_t){19,0} //dot

#define D18 (lcd_seg_t){20,0}
#define E18 (lcd_seg_t){20,1}
#define G18 (lcd_seg_t){20,2}
#define F18 (lcd_seg_t){20,3}
#define C18 (lcd_seg_t){21,1}
#define B18 (lcd_seg_t){21,2}
#define A18 (lcd_seg_t){21,3}

#define HZ2 (lcd_seg_t){21,0}
#define S28 (lcd_seg_t){22,0} //%
#define S27 (lcd_seg_t){22,1} //V
#define S31 (lcd_seg_t){22,2} //A
#define S30 (lcd_seg_t){22,3}//W

#define S32 (lcd_seg_t){23,0}//lines under
#define S62 (lcd_seg_t){23,1}//bypass
#define S63 (lcd_seg_t){23,2}//sound off
#define S55 (lcd_seg_t){23,3}//overload

#define S53 (lcd_seg_t){24,0}//P
#define S52 (lcd_seg_t){24,1}//
#define S54 (lcd_seg_t){24,2}//lamp

#define S56 (lcd_seg_t){24,3}//scale ramka
#define S58 (lcd_seg_t){25,0}//scale 100%
#define S59  (lcd_seg_t){25,1}// scale 75%
#define S60  (lcd_seg_t){25,2}//scale 50%
#define S61  (lcd_seg_t){25,3}//scale 25%

#define S36 (lcd_seg_t){26,0}//
#define S49 (lcd_seg_t){26,1}//
#define S50 (lcd_seg_t){26,2}//
#define S51 (lcd_seg_t){26,3}//

#define S40 (lcd_seg_t){27,0}//
#define S47 (lcd_seg_t){27,1}//
#define S41 (lcd_seg_t){27,2}//
#define S48 (lcd_seg_t){27,3}//

#define S39 (lcd_seg_t){28,0}//
#define S38 (lcd_seg_t){28,1}//
#define S46 (lcd_seg_t){28,2}// battary ramka
#define S57 (lcd_seg_t){28,3}// charging

#define S45 (lcd_seg_t){29,0}// battary line
#define S44 (lcd_seg_t){29,1}// battary line
#define S43 (lcd_seg_t){29,2}// battary line
#define S42 (lcd_seg_t){29,3}// battary line

#define S35 (lcd_seg_t){30,0}//
#define S37 (lcd_seg_t){30,1}//
#define S33 (lcd_seg_t){30,2}// wind
#define S34 (lcd_seg_t){30,3}// line 220

#define S67 (lcd_seg_t){31,0}// solar line
#define S66 (lcd_seg_t){31,1}// solar line
#define S65 (lcd_seg_t){31,2}// solar line
#define S64 (lcd_seg_t){31,3}// solar ramka

#endif /* LCD_MAP_H_ */
