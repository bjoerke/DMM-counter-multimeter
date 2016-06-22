#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdbool.h>

#include "systime.h"

#include "joy.h"

//higher values: more delay, less errors
#define DEBOUNCE_THRESHOLD 20

#define JOY_PIN			PINA
#define JOY_DDR			DDRA
#define JOY_PORT		PORTA

#define JOY_MASK 		(JOY_UP | JOY_DOWN | JOY_LEFT | JOY_RIGHT | JOY_PUSH)

static volatile uint8_t current_state = JOY_NONE;
static uint8_t counter = 0;
static volatile bool state_changed = false;

static void joy_Update(void) {
	uint8_t state = (~JOY_PIN) & JOY_MASK;
	if(current_state != state)
	{
		counter++;
		if(counter > DEBOUNCE_THRESHOLD)
		{
			current_state = state;
			state_changed = true;
			counter = 0;
		}
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

	// set timer callback
	time_RegisterCallback(joy_Update);
}

uint8_t joy_Wait(void)
{
	cli(); state_changed = false; sei();
	while(! state_changed)
		;
	return current_state;
}

uint8_t joy_GetState(void)
{
	return current_state;
}
