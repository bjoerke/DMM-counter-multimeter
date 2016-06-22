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
#include "gui.h"
#include "joy.h"
#include "multimeter.h"
#include "systime.h"
#include "counterHAL.h"

void uart_rx_listener(char c)
{
	UART_PutChar(c);
}

void do_measure(uint8_t function)
{
	uint8_t dummy;
	switch(function)
	{
		case ID_VOLTMETER_AUTO:
			// Hier muss noch ein Funktionsaufruf zu meter_measure(...) erfolgen, der die Paramter "VOLTAGE" und "AUTO" �bergibt
			break;
		case ID_VOLTMETER_500V:
			dummy = meter_measure(0x01, 0x01);			// 0x01 = parameter voltage / 0x01 = rangemode1		*******meter_measure liefert einen int32_t zurueck!*******
			//DMM_SetURange(DMM_RANGE_500V);
			break;
		case ID_VOLTMETER_200V:
			dummy = meter_measure(0x01, 0x02);			// 0x01 = parameter voltage / 0x02 = rangemode2		*******meter_measure liefert einen int32_t zurueck!*******
			//DMM_SetURange(DMM_RANGE_200V);
			break;
		case ID_VOLTMETER_20V:
			dummy = meter_measure(0x01, 0x04);			// 0x01 = parameter voltage / 0x04 = rangemode3		*******meter_measure liefert einen int32_t zurueck!*******
			//DMM_SetURange(DMM_RANGE_20V);
			break;
		case ID_VOLTMETER_2V:
			dummy = meter_measure(0x01, 0x08);			// 0x01 = parameter voltage / 0x08 = rangemode4		*******meter_measure liefert einen int32_t zurueck!*******
			//DMM_SetURange(DMM_RANGE_2V);
			break;
		case ID_AMPEREMETER_10A:
			DMM_SetIRange(DMM_RANGE_10A);
			break;
		case ID_AMPEREMETER_200mA:
			DMM_SetIRange(DMM_RANGE_200mA);
			break;
		case ID_AMPEREMETER_20mA:
			DMM_SetIRange(DMM_RANGE_20mA);
			break;
		case ID_AMPEREMETER_2mA:
			DMM_SetIRange(DMM_RANGE_2mA);
			break;
		case ID_AMPEREMETER_200uA:
			DMM_SetIRange(DMM_RANGE_200uA);
			break;
		case ID_WIDERSTAND_20M:
			DMM_SetRRange(DMM_RANGE_20MOhm);
			break;
		case ID_WIDERSTAND_2M:
			DMM_SetRRange(DMM_RANGE_2MOhm);
			break;
		case ID_WIDERSTAND_200K:
			DMM_SetRRange(DMM_RANGE_200kOhm);
			break;
		case ID_WIDERSTAND_20K:
			DMM_SetRRange(DMM_RANGE_20kOhm);
			break;
		case ID_WIDERSTAND_2K:
			DMM_SetRRange(DMM_RANGE_2kOhm);
			break;
		case ID_DURCHGANG:
			break;
		case ID_FREQUENZ:
			while (joy_GetState() != JOY_LEFT)
			{
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
			break;
	}
}

int main(void)
{
	// set PA3-PA7 as input and activated internal Pull-Up
	// PA3-PA7 are the joystick-connected pins.
	DDRA &= ~((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
			| (1 << PINA7));		// Required for DMM Board 2013
	PORTA |= ((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
			| (1 << PINA7));		// Required for DMM Board 2013

	// set PB0-PB3 as output
	DDRB = 0x0F;
	// set PB0-PB3 on high-level (internal Pull-Up on)
	PORTB |= 0x0F;		// Required for DMM Board DMM Board 2013

	LCD_Init();
	UART_Init();

	time_Init();
	joy_Init();
	shift_Init();
	counter_Init();
	ADC_Init();

	UART_SetDataReceivedListener(uart_rx_listener);

	counter_SelectInput(CNT_IN_LF, CNT_LF_PRE_1);

	// Enable interrupts:
	sei();
	
	gui_Init();
	gui_Welcome();
	while(1)
	{
		uint8_t function = gui_SelectFunction();
		do_measure(function);
		// TODO: Prepare PINA0...2 for input
	}

	// ###########################
	// ## Begin of the old code ##
	// ###########################

	//uint8_t range = 1;
//
	//while (1) 
	//{
		//LCD_WipeLine(2);
		//LCD_GotoXY(0, 2);
		//if (range < 5)
		//{
			//switch (range)
			//{
				//case DMM_RANGE_500V:
				//case DMM_RANGE_200V:
				//case DMM_RANGE_20V:
				//case DMM_RANGE_2V:
					//DMM_SetURange(range);
					//break;
			//}
			//LCD_PutString("Voltage ");
			//LCD_PutChar(range + '0');
		//}
		//else if (range < 10)
		//{
			//switch (range - 4)
			//{
				//case DMM_RANGE_10A:
				//case DMM_RANGE_200mA:
				//case DMM_RANGE_20mA:
				//case DMM_RANGE_2mA:
				//case DMM_RANGE_200uA:
					//DMM_SetIRange(range - 4);
					//break;
			//}
			//LCD_PutString("Current ");
			//LCD_PutChar(range - 4 + '0');
		//}
		//else if (range < 15)
		//{
			//switch (range - 9)
			//{
				//case DMM_RANGE_20MOhm:
				//case DMM_RANGE_2MOhm:
				//case DMM_RANGE_200kOhm:
				//case DMM_RANGE_20kOhm:
				//case DMM_RANGE_2kOhm:
					//DMM_SetRRange(range - 9);
					//break;
			//}
			//LCD_PutString("Resistance ");
			//LCD_PutChar(range - 9 + '0');
		//}
		//else if (range < 16)						// Continuity tester
		//{
			//switch (range - 14)
			//{
				//case DMM_RANGE_CONTIN:
					//DMM_SetCRange();
					//break;
			//}
			//LCD_PutString("Continuity ");
			//LCD_PutChar(range - 15 + '0');
		//}
		//LCD_Update();
		//uint8_t oldrange = range;
		//do
		//{
			//if (!(PINA & (1 << PA6)))
			//{
				//// DOWN button pressed
				//if (range < 15)
					//range++;
			//}
			//if (!(PINA & (1 << PA7)))
			//{
				//// UP button pressed
				//if (range > 1)
					//range--;
			//}
		//} while (oldrange == range);
		//// wait for buttons to be released
		//while ((PINA & 0xC0) != 0xC0)
			//;
	//}

// ##################################
// ## Hier beginnt der Z�hler-Teil ##
// ##################################

    //Backlight_LED(BL_BLUE_ON);
//
    //LCD_PutString_P(PSTR("Counter test"));
//
    //LCD_Update();
//
    //time_Init();
    //shift_Init();
    //counter_Init();
    //
    //UART_SetDataReceivedListener(uart_rx_listener);
    //sei();
    //
    //counter_SelectInput(CNT_IN_LF, CNT_LF_PRE_1);
    //while (1) {
	    //// count impulses on PB0 for one second
	    //PORTB |= (1 << PB2);
	    //uint32_t freq = counter_MeasureRefGate(CNT_GATE_MS(1000));
	    //PORTB &= ~(1 << PB2);
	    //// display measurement on lcd
	    //char buf[11];
	    //buf[10] = '\0';
	    //int8_t i;
	    //for (i = 9; i >= 0; i--) {
		    //buf[i] = (freq % 10) + '0';
		    //freq /= 10;
	    //}
	    //LCD_WipeLine(2);
	    //LCD_GotoXY(0, 2);
	    //LCD_PutString_P(PSTR("HZ:"));
	    //LCD_PutString(buf);
	    //LCD_Update();
	    //UART_PutString("Hello World!\n");
	    //time_Waitms(200);
    //}
}
