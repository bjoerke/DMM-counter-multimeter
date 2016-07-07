/*
 * CFile1.c
 *
 * Created: 02.06.2016 01:41:49
 *  Author: Bjoern
 */

#include <stdlib.h>
#include <stdbool.h>
#include "uartProtocol.h"
#include "includes/uart.h"

#define UP_PREAMBLE 0xAA
#define UP_ADDR_MULTIMETER 1
#define UP_ADDR_LOCKIN 2
#define UP_ADDR_COUNTER 3

#define UP_INDEX_PREAMBLE 0
#define UP_INDEX_ADDRESS 1
#define UP_INDEX_DATA 2
#define UP_INDEX_CHECKSUM 162
#define UP_INDEX_CR 163

static request requestBuffer;
static uint8_t index;
static uint8_t checksum;
static volatile bool rx_ready;

static void reset_rx() {
	index = UP_INDEX_PREAMBLE;
	checksum = 0;
	rx_ready = false;
}

static void uart_rx_listener(char c) {
	switch (index) {
	case UP_INDEX_PREAMBLE:
		if (c != UP_PREAMBLE) {
			reset_rx();
			return;
		}
		break;
	case UP_INDEX_ADDRESS:
		if (c != UP_ADDR_MULTIMETER) {
			reset_rx();
			return;
		}
		break;
	case UP_INDEX_CHECKSUM:
		if (c != checksum) {
			LCD_PutString("\n\rcheck!!!!");
			LCD_Update();
			reset_rx();
			return;
		}
		break;
	case UP_INDEX_CR:
		reset_rx();
		if (c == 0x0D) {
			rx_ready = true;
		}
		return;
	default: //Data frame
	{
		uint8_t i = index - UP_INDEX_DATA;
		if (i < sizeof(request)) {
			((uint8_t *) &requestBuffer)[i] = c;
		}
		checksum ^= c;
		break;
	}
	}
	index++;
}

void uartProtocol_Init() {
	reset_rx();
	UART_Init();
	UART_SetDataReceivedListener(uart_rx_listener);
}

request* uartProtocol_WaitRequest(uint16_t timeout) {
	uint32_t endtime = time_Getms() + timeout;
	while (!rx_ready && (endtime > time_Getms() || !timeout))
		;
	if (rx_ready) {
		rx_ready = false;
		return &requestBuffer;
	} else {
		return NULL ;
	}
}

void uartProtocol_SendResponse(response* resp) {
	uint8_t _checksum = 0;
	UART_PutChar(UP_PREAMBLE);
	UART_PutChar(UP_ADDR_MULTIMETER);
	for (int i = 0; i < sizeof(response); i++) {
		uint8_t data = ((uint8_t*) resp)[i];
		UART_PutChar(data);
		_checksum ^= data;
	}
	for (int i = sizeof(response); i < UP_INDEX_CHECKSUM - UP_INDEX_DATA; i++) {
		UART_PutChar(0);
	}
	UART_PutChar(_checksum);
	UART_PutChar(0x0D);
}
