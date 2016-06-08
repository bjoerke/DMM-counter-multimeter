

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "joy.h"

extern uint8_t page;
extern uint8_t newpage;

char wait_joy_button()
{
	switch((PINA)&0x28)
	{
		case 0x20: //push Button
		time_Waitms(5);
		if (((PINA)&0x28)==0x20)
		{
			return 0;
		} 
		else
		{
			return 1;
		}
		default:
		return 1;
	}
}

char long_joy_button()
{
	if ((PINA&0x08)==0)
	{
		time_Waitms(1000);
		if ((PINA&0x08)==0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
} 

char long_forward_button()
{
	if ((PINA&0x10)==0)
	{
		time_Waitms(1000);
		if ((PINA&0x10)==0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}
}

char wait_back_button()
{
	switch((PINA)&0x38)	
	{
		case 0x18://left Button
		time_Waitms(5);
		if (((PINA)&0x38)==0x18)
		{
			LCD_Clear();
			switch(page)
			{
				case 0x10:		// Voltmeter Hauptauswahl
					newpage = 0x00;
					break;
				case 0x11:		// Voltmeter Auto
					newpage = 0x10;
					break;
				case 0x12:		// Voltmeter Manuell Auswahl
					newpage = 0x10;
					break;
				case 0x13:		// Voltmeter 500V
					newpage = 0x12;
					break;
				case 0x14:		// Voltmeter 200V
					newpage = 0x12;
					break;
				case 0x15:		// Voltmeter 20V
					newpage = 0x12;
					break;
				case 0x16:		// Voltmeter 2V
					newpage = 0x12;
					break;
				case 0x20:		// Amperemeter Hauptauswahl
					newpage = 0x00;
					break;
				case 0x21:		// Amperemeter Auto
					newpage = 0x20;
					break;
				case 0x22:		// Amperemeter-Manuell Auswahl
					newpage = 0x20;
					break;
				case 0x23:		// Amperemeter 10A
					newpage = 0x22;
					break;
				case 0x24:		// Amperemeter 200mA
					newpage = 0x22;
					break;
				case 0x25:		// Amperemeter 20mA
					newpage = 0x22;
					break;
				case 0x26:		// Amperemeter 2mA
					newpage = 0x22;
					break;
				case 0x27:		// Amperemeter 20uA
					newpage = 0x22;
					break;
				case 0x40:		// Widerstand (Auto oder Manuell Auswahl)
					newpage = 0x00;
					break;
				case 0x41:		// Widerstand-Auto
					newpage = 0x40;
					break;
				case 0x42:		// Widerstand-Manuell Auswahl
					newpage = 0x40;
					break;
				case 0x43:		// Widerstand 20M
					newpage = 0x42;
					break;
				case 0x44:		// Widerstand 2M
					newpage = 0x42;
					break;
				case 0x45:		// Widerstand 200k
					newpage = 0x42;
					break;
				case 0x46:		// Widerstand 20k
					newpage = 0x42;
					break;
				case 0x47:		// Widerstand 2k
					newpage = 0x42;
					break;
				case 0x80:		// Durchgangspruefung
					newpage = 0x00;
					break;
				case 0x90:		// Frequenzmessung
					newpage = 0x00;
					break;
			}
			return 0;
		}
		else
		{
			return 1;
		}
		default:
		return 1;
	}
}

void wait_forward_button()
{
	switch((PINA)&0x18)
	{
		case 0x08://left Button
		time_Waitms(5);
		if (((PINA)&0x18)==0x08)
		{
			//page_temp=page;
			//page=20;
		}
		break;
		default:
		break;
	}	
}