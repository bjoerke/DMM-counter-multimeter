#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "includes/uart.h"
#include "includes/lcd.h"
#include "includes/twi.h"
#include "multimeter.h"
#include "systime.h"
#include "counterHAL.h"

int main(void) {
	// set PA3-PA7 as input and activated internal Pull-Up
	DDRA &= ~((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
			| (1 << PINA7));		// Required for DMM Board 2013
	PORTA |= ((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
			| (1 << PINA7));		// Required for DMM Board 2013

	// set PB0-PB3 as output
	DDRB = 0x0F;
	// set PB0-PB3 on high-level
	PORTB |= 0x0F;		// Required for DMM Board DMM Board 2013

	LCD_Init();
	UART_Init();

//	LCD_Clear();
//	LCD_PutString("Multimeter Rangetest");
//	LCD_Update();
//
//	Backlight_LED(BL_BLUE_ON | BL_GREEN_ON | BL_RED_ON);
//
//	time_Init();
//	shift_Init();
//	sei();
//
//	uint8_t range = 1;
//
//	while (1) {
//		LCD_WipeLine(2);
//		LCD_GotoXY(0, 2);
//		if (range < 5) {
//			switch (range) {
//			case DMM_RANGE_500V:
//			case DMM_RANGE_200V:
//			case DMM_RANGE_20V:
//			case DMM_RANGE_2V:
//				DMM_SetURange(range);
//				break;
//			}
//			LCD_PutString("Voltage ");
//			LCD_PutChar(range + '0');
//		} else if (range < 10) {
//			switch (range - 4) {
//			case DMM_RANGE_10A:
//			case DMM_RANGE_200mA:
//			case DMM_RANGE_20mA:
//			case DMM_RANGE_2mA:
//			case DMM_RANGE_200uA:
//				DMM_SetIRange(range - 4);
//				break;
//			}
//			LCD_PutString("Current ");
//			LCD_PutChar(range - 4 + '0');
//		} else if (range < 15) {
//			switch (range - 9) {
//			case DMM_RANGE_20MOhm:
//			case DMM_RANGE_2MOhm:
//			case DMM_RANGE_200kOhm:
//			case DMM_RANGE_20kOhm:
//			case DMM_RANGE_2kOhm:
//				DMM_SetRRange(range - 9);
//				break;
//			}
//			LCD_PutString("Resistance ");
//			LCD_PutChar(range - 9 + '0');
//		}
//		LCD_Update();
//		uint8_t oldrange = range;
//		do {
//			if (!(PINA & (1 << PA6))) {
//				// DOWN button pressed
//				if (range < 14)
//					range++;
//			}
//			if (!(PINA & (1 << PA7))) {
//				// UP button pressed
//				if (range > 1)
//					range--;
//			}
//		} while (oldrange == range);
//		// wait for buttons to be released
//		while ((PINA & 0xC0) != 0xC0)
//			;
//
//	}

    Backlight_LED(BL_BLUE_ON);

    LCD_PutString_P(PSTR("Counter test"));

    LCD_Update();

    time_Init();
    shift_Init();
    counter_Init();
    sei();
	
    counter_SelectInput(CNT_IN_LF, CNT_LF_PRE_1);
    while (1) {
        // count impulses on PB0 for one second
        PORTB |= (1 << PB2);
        uint32_t freq = counter_MeasureRefGate(CNT_GATE_MS(1000));
        PORTB &= ~(1 << PB2);
        // display measurement on lcd
        char buf[11];
        buf[10] = '\0';
        int8_t i;
        for (i = 9; i >= 0; i--) {
            buf[i] = (freq % 10) + '0';
            freq /= 10;
        }
        LCD_WipeLine(2);
        LCD_GotoXY(0, 2);
        LCD_PutString_P(PSTR("HZ:"));
        LCD_PutString(buf);
        LCD_Update();
		UART_PutString("Hello World!\n");
        time_Waitms(200);
    }
}

