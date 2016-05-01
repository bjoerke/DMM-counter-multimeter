#include "systime.h"

void time_Init(void) {
    time.ms = 0;
    TCCR3A = TCCR3B = 0;
    TIMSK3 = 0;
    // Configure timer 3 to overflow each ms
    TCCR3B |= (1 << WGM32); 	// CTC mode
    OCR3A = 249;			// 250 counts to overflow
    TCCR3B |= (1 << CS31) | (1 << CS30);	// prescaler = 64
    TIMSK3 |= (1 << OCIE3A);	// enable timer overflow interrupt
}

uint32_t time_Getms(void) {
    TIMSK2 &= ~(1 << OCIE2A);// block timer interrupt to avoid data corruption
    uint32_t ret = time.ms;
    TIMSK2 |= (1 << OCIE2A);
    return ret;
}

void time_Waitms(uint16_t ms) {
    uint32_t start = time_Getms();
    while (time_Getms() < start + ms)
        ;
}

ISR(TIMER3_COMPA_vect) {
    // timer 3 overflowed -> 1 ms has passed
    time.ms++;
}
