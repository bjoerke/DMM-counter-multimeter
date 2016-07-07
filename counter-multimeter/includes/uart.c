#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "uart.h"
#include "fifo.h"

#define UART_BUFSIZE_IN      16
#define UART_BUFSIZE_OUT     64

uint8_t uart_inbuf[UART_BUFSIZE_IN];
uint8_t uart_outbuf[UART_BUFSIZE_OUT];

fifo_t uart_infifo;
fifo_t uart_outfifo;

void (*uart_data_received_listener)(char) = NULL;

#ifdef DEBUG

/** select UART 0 (connected to USB) **/
#define UBRRn   UBRR0
#define UCSRnA  UCSR0A
#define UCSRnB  UCSR0B
#define RXENn   RXEN0
#define TXENn   TXEN0
#define RXCIEn  RXCIE0
#define RXCn    RXC0
#define TXCn    TXC0
#define UDRIEn  UDRIE0
#define USARTn_RX_vect USART0_RX_vect
#define UDRn   UDR0
#define USARTn_UDRE_vect USART0_UDRE_vect

#else

/** select UART 1 **/
#define UBRRn   UBRR1
#define UCSRnA  UCSR1A
#define UCSRnB  UCSR1B
#define RXENn   RXEN1
#define TXENn   TXEN1
#define RXCIEn  RXCIE1
#define RXCn    RXC1
#define TXCn    TXC1
#define UDRIEn  UDRIE1
#define USARTn_RX_vect USART1_RX_vect
#define UDRn     UDR1
#define USARTn_UDRE_vect USART1_UDRE_vect

#endif

void
UART_Init (void)
{
  // Save Status Register and disable Interrupts
  uint8_t sreg = SREG;
  cli();

  
  #ifdef DEBUG
   // Set Baudrate according to datasheet (16MHz -> 9600 Baud -> 103)
    UBRRn = 103;
  #else
	UBRRn = 1;  //0.5M Baud rate
  #endif

  // Enable RX, TX and RX Complete Interrupt
  UCSRnB = (1 << RXENn)|(1 << TXENn)|(1 << RXCIEn);

  // Reset Complete-Flags
  UCSRnA = (1 << RXCn)|(1 << TXCn);

  // Reset Status Register
  SREG = sreg;

  // Initialize FIFO Buffers
  fifo_init(&uart_infifo, uart_inbuf, UART_BUFSIZE_IN);
  fifo_init(&uart_outfifo, uart_outbuf, UART_BUFSIZE_OUT);
}

void
UART_SetDataReceivedListener(void(*listener)(char))
{
	uart_data_received_listener = listener;
}

int8_t
UART_PutChar (const uint8_t c)
{



  // Put char into TX Buffer
  int8_t ret;
  do {
	  ret = fifo_put(&uart_outfifo, c);
  } while(!ret);

  // Enable DRE Interrupt
  UCSRnB |= (1 << UDRIEn);

  return ret;
}

// Receive Interrupt Routine
ISR(USARTn_RX_vect)
{
  if(uart_data_received_listener == NULL)
  {
     fifo_put(&uart_infifo, UDRn);
  }
  else
  {
	  uart_data_received_listener(UDRn);
  }
}

// Data Register Empty Interrupt
ISR(USARTn_UDRE_vect)
{
  if (uart_outfifo.count > 0)
    UDRn = fifo_get_nowait(&uart_outfifo);
  else
    UCSRnB &= ~(1 << UDRIEn);
}

int16_t
UART_GetChar (void)
{
  return fifo_get_nowait(&uart_infifo);
}

uint8_t
UART_GetChar_Wait (void)
{
  return fifo_get_wait(&uart_infifo);
}

void
UART_PutString (const char *s)
{
	while (*s)
    {
      UART_PutChar(*s++);
    }

}

void
UART_PutInteger (const int i)
{
  // Buffer for Output
  char buffer[10];

  // Convert Integer to ASCII, Base 10
  itoa(i, buffer, 10);

  UART_PutString(buffer);
}

void UART_PutLongInteger (const uint32_t i){
	  // Buffer for Output
	  char buffer[13];

	  // Convert Integer to ASCII, Base 10
	  ltoa(i, buffer, 10);

	  UART_PutString(buffer);
}
