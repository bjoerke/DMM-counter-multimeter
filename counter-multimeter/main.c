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

    Backlight_LED(BL_BLUE_ON);

    LCD_PutString_P(PSTR("Counter test"));

    LCD_Update();

    time_Init();
    shift_Init();
    counter_Init();
    sei();

    while (1) {
        // count impulses on PB0 for one second
        PORTB |= (1 << PB2);
        uint32_t freq = counter_MeasureRefGate(CNT_GATE_MS(1000));
        PORTB &= ~(1 << PB2);
        // display measurement on lcd
        char buf[11];
        buf[10] = '\0';
        int8_t i;
        for (i = 9; i >= 0; i--) {
            buf[i] = (freq % 10) + '0';
            freq /= 10;
        }
        LCD_WipeLine(2);
        LCD_GotoXY(0, 2);
        LCD_PutString_P(PSTR("HZ:"));
        LCD_PutString(buf);
        LCD_Update();
        time_Waitms(200);
    }
}
