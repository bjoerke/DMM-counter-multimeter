#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>

#define JOY_UP_PIN		PA7
#define JOY_RIGHT_PIN 	PA4
#define JOY_DOWN_PIN	PA6
#define JOY_LEFT_PIN	PA5
#define JOY_PUSH_PIN	PA3

#define JOY_UP			(1<<JOY_UP_PIN)
#define JOY_RIGHT		(1<<JOY_RIGHT_PIN)
#define JOY_DOWN		(1<<JOY_DOWN_PIN)
#define JOY_LEFT		(1<<JOY_LEFT_PIN)
#define JOY_PUSH		(1<<JOY_PUSH_PIN)
#define JOY_NONE        0x00

/**
 * \brief Initializes joystick
 */
void joy_Init(void);

/**
 * Wait for a button to be pressed
 * @return id of pressed button
 */
uint8_t joy_Wait(void);

/**
 * Gets the pressed button if any
 * @return the currently pressed button (or JOY_NONE)
 */
uint8_t joy_GetState(void);