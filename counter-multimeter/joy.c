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

char wait_joy_button() {
	if (joy_State(JOY_PUSH))
		return 1;
	else
		return 0;
}

char wait_back_button() {
	if (!joy_State(JOY_LEFT)) {
		switch (page) {
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
	} else {
		return 1;
	}
}

void joy_Init(void) {
	// configure pins as inputs with pull-up
	JOY_DDR &= ~(1 << JOY_UP_PIN);
	JOY_DDR &= ~(1 << JOY_RIGHT_PIN);
	JOY_DDR &= ~(1 << JOY_DOWN_PIN);
	JOY_DDR &= ~(1 << JOY_LEFT_PIN);
	JOY_DDR &= ~(1 << JOY_PUSH_PIN);

	JOY_PORT |= (1 << JOY_UP_PIN);
	JOY_PORT |= (1 << JOY_RIGHT_PIN);
	JOY_PORT |= (1 << JOY_DOWN_PIN);
	JOY_PORT |= (1 << JOY_LEFT_PIN);
	JOY_PORT |= (1 << JOY_PUSH_PIN);

	joy.state = 0x00;
	joy.pressed = 0x00;

	// set timer callback
	time_RegisterCallback(joy_Update);
}

uint8_t joy_Pressed(uint8_t joymask) {
	cli();
	joymask &= joy.pressed;
	joy.pressed ^= joymask;
	sei();
	return joymask;
}

uint8_t joy_State(uint8_t joymask) {
	joymask &= joy.state;
	return joymask;
}

void joy_Update(void) {
	static uint8_t ct0 = 0xFF, ct1 = 0xFF;
	uint8_t i;

	i = joy.state ^ ~(JOY_PIN & JOY_MASK);                  // key changed ?
	ct0 = ~(ct0 & i);                             // reset or count ct0
	ct1 = ct0 ^ (ct1 & i);                          // reset or count ct1
	i &= ct0 & ct1;                               // count until roll over ?
	joy.state ^= i;                           // then toggle debounced state
	joy.pressed |= joy.state & i;                  // 0->1: key press detect
}
