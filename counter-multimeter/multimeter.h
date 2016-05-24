#ifndef MULTIMETER_H_
#define MULTIMETER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
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

#endif
