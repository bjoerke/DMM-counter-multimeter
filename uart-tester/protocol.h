#ifndef _PROTO_H_
#define _PROTO_H_

#include <stdint.h>

#define UP_ADDRESS 0x55AA

typedef struct __attribute__((packed))
{
	uint16_t address;
	uint8_t direct_voltage;
	uint8_t direct_current;
	uint8_t alternating_voltage;
	uint8_t alternating_current;
	uint8_t resistance;
	uint8_t frequency;
	uint8_t duty_cycle;
	uint8_t _reserved;
	uint8_t checksum;
} request;

#define UP_DONT_MEASURE 0x00
#define UP_MEASURE_RANGE_AUTO 0xFF

typedef struct  __attribute__((packed))
{
	uint16_t address;
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
	uint8_t _reserved[5];
	uint8_t checksum;
} response;

void protocol_calc_checksum(request* r);

#endif // _PROTO_H_