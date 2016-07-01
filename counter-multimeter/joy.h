#ifndef _JOY_H_
#define _JOY_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "systime.h"
#include "gui.h"

#define JOY_PIN			PINA
#define JOY_DDR			DDRA
#define JOY_PORT		PORTA

#define JOY_UP_PIN		PA7
#define JOY_RIGHT_PIN 	PA4
#define JOY_DOWN_PIN	PA6
#define JOY_LEFT_PIN	PA5
#define JOY_PUSH_PIN	PA3

#define JOY_MASK 		((1<<JOY_UP_PIN)|(1<<JOY_RIGHT_PIN)|(1<<JOY_DOWN_PIN)|(1<<JOY_LEFT_PIN)|(1<<JOY_PUSH_PIN))

#define JOY_UP			0x80
#define JOY_RIGHT		0x10
#define JOY_DOWN		0x40
#define JOY_LEFT		0x20
#define JOY_PUSH		0x08

struct {
	uint8_t state;
	uint8_t pressed;
} joy;

/**
 * \brief Initializes joystick
 */
void joy_Init(void);

/**
 * \brief Reports if a joystick button has been pressed
 *
 * Each press is only reported once.
 *
 * \param joymask Joystick button that will be checked.
 * \return Mask of pressed buttons
 */
uint8_t joy_Pressed(uint8_t joymask);

/**
 * \brief Reports if a joystick button is currently pressed
 *
 * \param joymask Joystick button that will be checked.
 * \return Mask of pressed buttons
 */
uint8_t joy_State(uint8_t joymask);

/**
 *  \brief Reads joystick pins, debounces and writes new state
 *
 *  see: https://www.mikrocontroller.net/articles/Entprellung
 */
void joy_Update(void);

#endif
