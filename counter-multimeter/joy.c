#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "joy.h"

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
