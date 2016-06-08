#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "includes/uart.h"
#include "includes/lcd.h"
#include "includes/twi.h"
#include "includes/dataflash.h"


void function_select();
void display_arrow(char row);
void scan_arrow();
void display_LCD(uint8_t p);
void Open();
