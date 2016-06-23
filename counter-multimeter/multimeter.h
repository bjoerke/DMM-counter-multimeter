#ifndef MULTIMETER_H_
#define MULTIMETER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "includes/lcd.h"
#include "systime.h"
#include "shiftreg.h"

#define DMM_RANGE_NONE		0x00

#define DMM_RANGE_AUTO_U	0x0F
#define DMM_RANGE_500V		0x04
#define DMM_RANGE_200V		0x03
#define DMM_RANGE_20V		0x02
#define DMM_RANGE_2V		0x01

#define DMM_RANGE_AUTO_I	0x1F
#define DMM_RANGE_10A		0x15
#define DMM_RANGE_200mA		0x14
#define DMM_RANGE_20mA		0x13
#define DMM_RANGE_2mA		0x12
#define DMM_RANGE_200uA		0x11

#define DMM_RANGE_AUTO_R	0x2F
#define DMM_RANGE_20MOhm	0x25
#define DMM_RANGE_2MOhm		0x24
#define DMM_RANGE_200kOhm	0x23
#define DMM_RANGE_20kOhm	0x22
#define DMM_RANGE_2kOhm		0x21

#define DMM_RANGE_CONTIN	0x3F

#define U_R_SUM				10000000	// Sum of the resistors: 10M
#define U_R_REF_500V		4000
#define U_R_REF_200V		10000
#define U_R_REF_20V			100000
#define U_R_REF_2V			1000000

#define I_R_SUM				1000.02
#define I_R_REF_10A			0.02	
#define I_R_REF_200mA		1.02
#define I_R_REF_20mA		10.02
#define I_R_REF_2mA			100.02
#define I_R_REF_200uA		1000.02

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
void DMM_SetCRange(uint8_t rRange);

/**
 * \brief Switches into continuity measurement mode
 *
 * \param range selects the range mode
 */
int32_t meter_TakeMeasurement(uint8_t range);

#endif
