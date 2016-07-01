#ifndef TIME_H_
#define TIME_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

struct {
	volatile uint32_t ms;
	void (*callback)(void);
	uint8_t beep;
} time;

/**
 * \brief Initializes system timer
 *
 * Configures timer 3 in CTC mode to overflow each millisecond
 */
void time_Init(void);

/**
 * \brief Sets a callback function that will be called every 10ms
 *
 * \param callback function pointer to void function
 */
void time_RegisterCallback(void (*callback)(void));

/**
 * \brief Reads the current system time since boot-up in milliseconds
 *
 * \return Time since boot-up in milliseconds
 */
uint32_t time_Getms(void);

/**
 * \brief Waits for specific amount of time
 *
 * \param ms Time to wait in milliseconds
 */
void time_Waitms(uint16_t ms);

#endif
