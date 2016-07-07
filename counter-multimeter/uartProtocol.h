/*
 * IncFile1.h
 *
 * Created: 02.06.2016 01:42:36
 *  Author: Bjoern
 */ 


#ifndef INCUART_PROTO_H_
#define INCUART_PROTO_H_

#include <stdint.h>
#include "systime.h"

typedef struct __attribute__((packed))
{
	uint8_t direct_voltage;
	uint8_t direct_current;
	uint8_t alternating_voltage;
	uint8_t alternating_current;
	uint8_t resistance;
	uint8_t frequency;
	uint8_t duty_cycle;
	uint8_t _reserved;
} request;

#define UP_DONT_MEASURE 0x00
#define UP_MEASURE_RANGE_AUTO 0xFF

typedef struct  __attribute__((packed))
{
	struct __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}direct_voltage;
	struct __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}direct_current;
	struct __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}alternating_voltage;
	struct  __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}alternating_current;
	struct  __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}resistance;
	struct  __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}frequency;
	struct  __attribute__((packed))
	{
		uint8_t range;
		uint32_t value;
	}duty_cycle;
} response;

/**
 * Initializes the UART to be used for the protocol
 */
void uartProtocol_Init(void);

/** 
 * Waits for a request
 * @param timeout Maximum wait time in ms (use 0 to wait indefinitely)
 * @return the request received
 */
request* uartProtocol_WaitRequest(uint16_t timeout);

/** 
 * Sends a response
 * @param response response to be send
 */
void uartProtocol_SendResponse(response* response);

#endif
