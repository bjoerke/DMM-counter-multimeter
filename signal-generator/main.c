#include <avr/io.h>
#include <util/delay.h>
 
#define DELAY_MS 1000
 
int main (void)
{
 DDRB |= _BV(DDB4);  //set pin for output
 
 while(1) {
  PORTB |= _BV(PORTB4);  //set pin high
  _delay_ms(DELAY_MS);
 
  PORTB &= ~_BV(PORTB4); //set pin low
  _delay_ms(DELAY_MS);
 }
}
