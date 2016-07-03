#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "uartProtocol.h"
#include "includes/lcd.h"
#include "includes/twi.h"
#include "multimeter.h"
#include "systime.h"
#include "counterHAL.h"
#include "gui.h"
#include "joy.h"

int main(void) {
	//// set PB0-PB3 as output
	//DDRB = 0x0F;
	//// set PB0-PB3 on high-level (internal Pull-Up on)
	//PORTB |= 0x0F;		// Required for DMM Board DMM Board 2013

	// set PB0, PB1, PB3 as output
	DDRB = 0x0B;		// 1011
	// set PB0, PB1, PB3 on high-level (internal Pull-Up on)
	PORTB |= 0x0B;		// Required for DMM Board DMM Board 2013
	// set PB2 as input
	DDRB &= ~(1 << PINB2);
	// set PB2 on high-level (internal Pull-Up on)
	PORTB |= (1 << PINB2);

	LCD_Init();
	uartProtocol_Init();

	time_Init();
	joy_Init();
	shift_Init();
	counter_Init();
	ADC_Init();

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

   /* UART protocol ussage
	while(1) {
		request* request =
		uartProtocol_WaitRequest();
		LCD_PutChar('#');
		LCD_Update();
		response response;
		uartProtocol_SendResponse(&response);
	} */
	
	while (1) {
		gui_DisplayMainMenu();
		time_Waitms(100);
		if (gui.selectedEntry < GUI_NUM_MEASUREMENT_ENTRIES
				&& gui.measurementActive) {
			gui_TakeMeasurement();
		} else {
			time.beep = 0;
			DMM_SetDefault();
			Backlight_LED(BL_BLUE_ON | BL_GREEN_ON | BL_RED_ON);
		}
		gui_HandleUserInput();
	}
}
