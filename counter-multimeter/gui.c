#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "gui.h"
#include "joy.h"
#include "systime.h"

static char arrow = 1;
extern uint8_t page;
extern uint8_t newpage;

void function_select() {
	while (!wait_joy_button() && !wait_back_button()) {
		scan_arrow();
		time_Waitms(100);
		display_arrow(arrow);
		time_Waitms(100);
	}
	//page=arrow-1;
	page = newpage;
	time_Waitms(200);
}

void display_arrow(char row) {
	switch (page) {
	case 0x00:
	case 0x10:
	case 0x12:
	case 0x20:
	case 0x22:
	case 0x40:
	case 0x42:
		for (int i = 0; i < 8; i++) //clear arrow
				{
			LCD_GotoXY(0, i);
			LCD_PutChar(0x20);
			LCD_Update();
		}
		if (page != 0x00) {
			LCD_GotoXY(0, 7);
			LCD_PutChar(0x3C);
			LCD_Update();
		}
		if (arrow > 1) {
			LCD_GotoXY(0, row);
			LCD_PutChar(0x10);
			if (page != 0x00) {
				LCD_GotoXY(0, 7);
				LCD_PutChar(0x3C);
			}
			LCD_Update();
		}
		break;
	}
}

void scan_arrow() {
	uint8_t dummy = 0;

	if (joy_State(JOY_UP)) {
		dummy = 0x80;
	}

	if (joy_State(JOY_DOWN)) {
		dummy = 0x40;
	}

	switch (page) {
	case 0x00:		// Hauptauswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 6) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x10;
			break;
		case 3:
			newpage = 0x20;
			break;
		case 4:
			newpage = 0x40;
			break;
		case 5:
			newpage = 0x80;
			break;
		case 6:
			newpage = 0x90;
			break;
		}
		break;
	case 0x10:		// Voltmeter Hauptauswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 3) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x11;
			break;
		case 3:
			newpage = 0x12;
			break;
		}
		break;
	case 0x11:		// Voltmeter Auto
		break;
	case 0x12:		// Voltmeter Manuell Auswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 5) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x13;
			break;
		case 3:
			newpage = 0x14;
			break;
		case 4:
			newpage = 0x15;
			break;
		case 5:
			newpage = 0x16;
			break;
		}
		break;
	case 0x13:		// Voltmeter 500V
		break;
	case 0x14:		// Voltmeter 200V
		break;
	case 0x15:		// Voltmeter 20V
		break;
	case 0x16:		// Voltmeter 2V
		break;
	case 0x20:		// Amperemeter Hauptauswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 3) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x21;
			break;
		case 3:
			newpage = 0x22;
			break;
		}
		break;
	case 0x21:		// Amperemeter Auto
		break;
	case 0x22:		// Amperemeter-Manuell Auswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 6) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x23;
			break;
		case 3:
			newpage = 0x24;
			break;
		case 4:
			newpage = 0x25;
			break;
		case 5:
			newpage = 0x26;
			break;
		case 6:
			newpage = 0x27;
			break;
		}
		break;
	case 0x23:		// Amperemeter 10A
		break;
	case 0x24:		// Amperemeter 200mA
		break;
	case 0x25:		// Amperemeter 20mA
		break;
	case 0x26:		// Amperemeter 2mA
		break;
	case 0x27:		// Amperemeter 20uA
		break;
	case 0x40:		// Widerstand (Auto oder Manuell Auswahl)
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 3) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x41;
			break;
		case 3:
			newpage = 0x42;
			break;
		}
		break;
	case 0x41:		// Widerstand-Auto
		break;
	case 0x42:		// Widerstand-Manuell Auswahl
		if ((arrow > 2) && (dummy == 0x80)) {
			arrow--;
		} else if ((arrow < 6) && (dummy == 0x40)) {
			arrow++;
		}
		switch (arrow) {
		case 2:
			newpage = 0x43;
			break;
		case 3:
			newpage = 0x44;
			break;
		case 4:
			newpage = 0x45;
			break;
		case 5:
			newpage = 0x46;
			break;
		case 6:
			newpage = 0x47;
			break;
		}
		break;
	case 0x43:		// Widerstand 20M
		break;
	case 0x44:		// Widerstand 2M
		break;
	case 0x45:		// Widerstand 200k
		break;
	case 0x46:		// Widerstand 20k
		break;
	case 0x47:		// Widerstand 2k
		break;
	case 0x80:		// Durchgangspruefung
		break;
	case 0x90:		// Frequenzmessung
		break;
	}

}

void display_LCD(uint8_t p) {
	LCD_GotoXY(0, 0);
	switch (p & 0xFF) {
	case 0x00: {
		LCD_PutString_P(PSTR("  HANDMULTIMETER     \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("  Voltmeter          \r\n"));
		LCD_PutString_P(PSTR("  Amperemeter        \r\n"));
		LCD_PutString_P(PSTR("  Widerstand         \r\n"));
		LCD_PutString_P(PSTR("  Durchgangspruefung \r\n"));
		LCD_PutString_P(PSTR("  Frequenzmessung    \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
	}
		arrow = 1;
		break;
	case 0x10:													// 0001 0000
	{
		LCD_PutString_P(PSTR("     Voltmeter       \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("  Messmodus Auto     \r\n"));		// 0001 0001
		LCD_PutString_P(PSTR("  Messmodus Manuell  \r\n"));		// 0001 0010
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x11:													// 0001 0001
	{
		LCD_PutString_P(PSTR("    Voltmeter-Auto   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		//LCD_PutString_P(PSTR("<       Oszillograph>\r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x12:													// 0001 0010
	{
		LCD_PutString_P(PSTR("  Voltmeter-Manuell  \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("   Messber. 500V     \r\n"));		// 0001 0011
		LCD_PutString_P(PSTR("   Messber. 200V	   \r\n"));		// 0001 0100
		LCD_PutString_P(PSTR("   Messber. 20V      \r\n"));		// 0001 0101
		LCD_PutString_P(PSTR("   Messber. 2V       \r\n"));		// 0001 0110
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x13:													// 0001 0011
	{
		LCD_PutString_P(PSTR("   Voltmeter 500V	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("               	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x14:													// 0001 0100
	{
		LCD_PutString_P(PSTR("   Voltmeter 200V	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("               	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x15:													// 0001 0101
	{
		LCD_PutString_P(PSTR("    Voltmeter 20V	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("               	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x16:													// 0001 0110
	{
		LCD_PutString_P(PSTR("     Voltmeter 2V	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("               	   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x20:													// 0010 0000
	{
		LCD_PutString_P(PSTR("     Amperemeter     \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("  Messmodus Auto     \r\n"));		// 0010 0001
		LCD_PutString_P(PSTR("  Messmodus Manuell  \r\n"));		// 0010 0010
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x21:													// 0010 0001
	{
		LCD_PutString_P(PSTR("   Amperemeter-Auto  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x22:													// 0010 0010
	{
		LCD_PutString_P(PSTR(" Amperemeter-Manuell \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("   Messber. 10A      \r\n"));		// 0010 0011
		LCD_PutString_P(PSTR("   Messber. 200mA    \r\n"));		// 0010 0100
		LCD_PutString_P(PSTR("   Messber. 20mA     \r\n"));		// 0010 0101
		LCD_PutString_P(PSTR("   Messber. 2mA      \r\n"));		// 0010 0110
		LCD_PutString_P(PSTR("   Messber. 200uA    \r\n"));		// 0010 0111
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x23:													// 0010 0011
	{
		LCD_PutString_P(PSTR("   Amperemeter 10A   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x24:													// 0010 0100
	{
		LCD_PutString_P(PSTR("  Amperemeter 200mA  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x25:													// 0010 0101
	{
		LCD_PutString_P(PSTR("   Amperemeter 20mA  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x26:													// 0010 0110
	{
		LCD_PutString_P(PSTR("   Amperemeter 2mA   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x27:													// 0010 0111
	{
		LCD_PutString_P(PSTR("  Amperemeter 200uA  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x40:													// 0100 0000
	{
		LCD_PutString_P(PSTR("     Widerstand      \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("  Messmodus Auto     \r\n"));		// 0100 0001
		LCD_PutString_P(PSTR("  Messmodus Manuell  \r\n"));		// 0100 0010
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x41:													// 0100 0001
	{
		LCD_PutString_P(PSTR("   Widerstand-Auto   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x42:													// 0100 0010
	{
		LCD_PutString_P(PSTR("  Widerstand-Manuell \r\n"));
		LCD_PutString_P(PSTR(" ------------------- \r\n"));
		LCD_PutString_P(PSTR("   Messb. 20MOhm     \r\n"));		// 0100 0011
		LCD_PutString_P(PSTR("   Messb. 2MOhm      \r\n"));		// 0100 0100
		LCD_PutString_P(PSTR("   Messb. 200kOhm    \r\n"));		// 0100 0101
		LCD_PutString_P(PSTR("   Messb. 20kOhm     \r\n"));		// 0100 0110
		LCD_PutString_P(PSTR("   Messb. 2kOhm      \r\n"));		// 0100 0111
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		arrow = 1;
		break;
	case 0x43:													// 0100 0011
	{
		LCD_PutString_P(PSTR("  Widerstand 20MOhm  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x44:													// 0100 0100
	{
		LCD_PutString_P(PSTR("  Widerstand 2MOhm   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x45:													// 0100 0101
	{
		LCD_PutString_P(PSTR("  Widerstand 200kOhm \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x46:													// 0100 0110
	{
		LCD_PutString_P(PSTR("  Widerstand 20kOhm  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x47:													// 0100 0111
	{
		LCD_PutString_P(PSTR("  Widerstand 2kOhm   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x80:													// 1000 0000
	{
		LCD_PutString_P(PSTR(" Durchgangspruefung  \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));
	}
		break;
	case 0x90:													// 1001 0000
	{
		LCD_PutString_P(PSTR("   Frequenzmessung   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("<                    \r\n"));

	}
		break;
	default:
		break;
	}
	LCD_Update();
}

/*
 void Open()
 {
 for(unsigned char i=0;i<31;i++)
 {
 LCD_DrawSquare(62-2*i,31-i,66+2*i,33+i,1);
 LCD_Update();
 _delay_ms(50);
 }
 for(unsigned char i=30;i!=0;i--)
 {
 LCD_DrawSquare(62-2*i,31-i,66+2*i,33+i,0);
 LCD_Update();
 _delay_ms(50);
 }

 }
 */
