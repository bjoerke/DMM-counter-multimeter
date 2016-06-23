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

void uart_rx_listener(char c) {
	UART_PutChar(c);
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
