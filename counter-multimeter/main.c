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
#include "gui.h"
#include "joy.h"

uint8_t page = 0;
uint8_t newpage;

void uart_rx_listener(char c) {
	UART_PutChar(c);
}

void do_measure(uint8_t page) {
	uint8_t dummy;
	switch (page) {
	case 0x00:		// Hauptauswahl
		break;
	case 0x10:		// Voltmeter Hauptauswahl
		break;
	case 0x11:		// Voltmeter Auto
		// Hier muss noch ein Funktionsaufruf zu meter_measure(...) erfolgen, der die Paramter "VOLTAGE" und "AUTO" �bergibt
		break;
	case 0x12:		// Voltmeter Manuell Auswahl
		break;
	case 0x13:		// Voltmeter 500V
		dummy = meter_measure(0x01, 0x01);// 0x01 = parameter voltage / 0x01 = rangemode1		*******meter_measure liefert einen int32_t zurueck!*******
		//DMM_SetURange(DMM_RANGE_500V);
		break;
	case 0x14:		// Voltmeter 200V
		dummy = meter_measure(0x01, 0x02);// 0x01 = parameter voltage / 0x02 = rangemode2		*******meter_measure liefert einen int32_t zurueck!*******
		//DMM_SetURange(DMM_RANGE_200V);
		break;
	case 0x15:		// Voltmeter 20V
		dummy = meter_measure(0x01, 0x04);// 0x01 = parameter voltage / 0x04 = rangemode3		*******meter_measure liefert einen int32_t zurueck!*******
		//DMM_SetURange(DMM_RANGE_20V);
		break;
	case 0x16:		// Voltmeter 2V
		dummy = meter_measure(0x01, 0x08);// 0x01 = parameter voltage / 0x08 = rangemode4		*******meter_measure liefert einen int32_t zurueck!*******
		//DMM_SetURange(DMM_RANGE_2V);
		break;
	case 0x20:		// Amperemeter Hauptauswahl
		break;
	case 0x21:		// Amperemeter Auto
		break;
	case 0x22:		// Amperemeter-Manuell Auswahl
		break;
	case 0x23:		// Amperemeter 10A
		DMM_SetIRange(DMM_RANGE_10A);
		break;
	case 0x24:		// Amperemeter 200mA
		DMM_SetIRange(DMM_RANGE_200mA);
		break;
	case 0x25:		// Amperemeter 20mA
		DMM_SetIRange(DMM_RANGE_20mA);
		break;
	case 0x26:		// Amperemeter 2mA
		DMM_SetIRange(DMM_RANGE_2mA);
		break;
	case 0x27:		// Amperemeter 20uA
		DMM_SetIRange(DMM_RANGE_200uA);
		break;
	case 0x40:		// Widerstand (Auto oder Manuell Auswahl)
		break;
	case 0x41:		// Widerstand-Auto
		break;
	case 0x42:		// Widerstand-Manuell Auswahl
		break;
	case 0x43:		// Widerstand 20M
		DMM_SetRRange(DMM_RANGE_20MOhm);
		break;
	case 0x44:		// Widerstand 2M
		DMM_SetRRange(DMM_RANGE_2MOhm);
		break;
	case 0x45:		// Widerstand 200k
		DMM_SetRRange(DMM_RANGE_200kOhm);
		break;
	case 0x46:		// Widerstand 20k
		DMM_SetRRange(DMM_RANGE_20kOhm);
		break;
	case 0x47:		// Widerstand 2k
		DMM_SetRRange(DMM_RANGE_2kOhm);
		break;
	case 0x80:		// Durchgangspruefung
		break;
	case 0x90:		// Frequenzmessung
		while (wait_back_button()) {
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

int main(void) {
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

	Backlight_LED(BL_BLUE_ON | BL_GREEN_ON | BL_RED_ON);

	LCD_Clear();
	LCD_PutString_P(PSTR("	                   \r\n"));
	LCD_PutString_P(PSTR("	                   \r\n"));
	LCD_PutString_P(PSTR("    Willkommen zur   \r\n"));
	LCD_PutString_P(PSTR("     Demo-Version    \r\n"));
	LCD_PutString_P(PSTR("         des         \r\n"));
	LCD_PutString_P(PSTR("    Counter-Meters   \r\n"));
	LCD_PutString_P(PSTR("                     \r\n"));
	LCD_PutString_P(PSTR("                     \r\n"));
	LCD_Update();
	time_Waitms(3000);

	// Enable interrupts:
	sei();

	while (1) {
		gui_DisplayMainMenu();
		time_Waitms(100);
		if (gui.selectedEntry < GUI_NUM_MEASUREMENT_ENTRIES
				&& gui.measurementActive) {
			gui_TakeMeasurement();
		}
		gui_HandleUserInput();
	}
}
