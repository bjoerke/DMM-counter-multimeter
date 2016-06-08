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

#include "systime.h"
#include "gui.h"

char wait_joy_button();
char long_joy_button();
char long_forward_button();
char wait_back_button();
void wait_forward_button();
