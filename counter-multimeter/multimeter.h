#ifndef MULTIMETER_H_
#define MULTIMETER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "includes/lcd.h"
#include "systime.h"
#include "shiftreg.h"

#define DMM_RANGE_10A		5
#define DMM_RANGE_200mA		4
#define DMM_RANGE_20mA		3
#define DMM_RANGE_2mA		2
#define DMM_RANGE_200uA		1

#define DMM_RANGE_500V		4
#define DMM_RANGE_200V		3
#define DMM_RANGE_20V		2
#define DMM_RANGE_2V		1

#define DMM_RANGE_20MOhm	5
#define DMM_RANGE_2MOhm		4
#define DMM_RANGE_200kOhm	3
#define DMM_RANGE_20kOhm	2
#define DMM_RANGE_2kOhm		1

#define DMM_RANGE_CONTIN	1

#define VOLTAGE		0x01		// 0000 0001
#define CURRENT		0x02		// 0000 0010
#define RESISTANCE	0x04		// 0000 0100
#define CONTINUITY	0x08		// 0000 1000

#define A_RANGE		0xFF		// Autorange				// 1111 1111
#define M_RANGE1	0x01		// Manual Rangemode 1		// 0000 0001
#define M_RANGE2	0x02		// Manual Rangemode 2		// 0000 0010
#define M_RANGE3	0x04		// Manual Rangemode 3		// 0000 0100
#define M_RANGE4	0x08		// Manual Rangemode 4		// 0000 1000
#define M_RANGE5	0x10		// Manual Rangemode 5		// 0001 0000

/*
 * positions of the DMM pins on the shift registers
 */
#define DMM_RANGESEL_LSB	7
#define DMM_RANGESEL_MSB	8
#define DMM_R_RANGE5		9
#define DMM_R_RANGE4		10
#define DMM_R_RANGE3		11
#define DMM_R_RANGE2		12
#define DMM_R_RANGE1		13
#define DMM_U_I_SELECTOR	14
#define DMM_R_SELECTOR		15

/**
 * \brief Switches into voltage measurement mode
 *
 * \param uRange Selected voltage range
 */
void DMM_SetURange(uint8_t uRange);

/**
 * \brief Switches into current measurement mode
 *
 * \param uRange Selected current range
 */
void DMM_SetIRange(uint8_t iRange);

/**
 * \brief Switches into resistance measurement mode
 *
 * \param uRange Selected resistance range
 */
void DMM_SetRRange(uint8_t rRange);

/**
 * \brief Switches into continuity measurement mode
 *
 * \param not necessary
 */
void DMM_SetCRange();

/**
 * \brief Switches into continuity measurement mode
 *
 * \param variable selects measurement variable
 * \param range selects the range mode
 */
int32_t meter_measure(uint8_t variable, uint8_t range);

#endif
