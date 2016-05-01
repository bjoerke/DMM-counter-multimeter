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
#include "counter.h"
#include "systime.h"

int main(void) {
    // set PA3-PA7 as input and activated internal Pull-Up
    DDRA &= ~((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
            | (1 << PINA7));		// Required for DMM Board 2013
    PORTA |= ((1 << PINA3) | (1 << PINA4) | (1 << PINA5) | (1 << PINA6)
            | (1 << PINA7));		// Required for DMM Board 2013

    // set PB0-PB3 as output
    DDRB = 0x0F;
    // set PB0-PB3 on high-level
    PORTB |= 0x0F;		// Required for DMM Board DMM Board 2013

    LCD_Init();
    UART_Init();

    time_Init();
    shift_Init();
    counter_Init();
    sei();

    while (1) {

    }
}
