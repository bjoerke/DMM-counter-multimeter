#ifndef COUNTER_H_
#define COUNTER_H_

#include "counterHAL.h"

#define COUNTER_DEF_SAMPLE_TIME			10
#define COUNTER_MAX_SAMPLE_TIME			2000
#define COUNTER_MAX_INPUTPIN_FREQ		2000000
#define COUNTER_MAX_CAPTURE_FREQUENCY	16000

#define COUNTER_DEF_PRECISION			6

/**
 * \name counter_ranges Available frequency ranges
 * \{
 */
#define COUNTER_RANGE_AUTO				0xFF
#define COUNTER_RANGE_NONE				0x00
#define COUNTER_RANGE_4MHz_TTL			0x01
#define COUNTER_RANGE_4MHz				0x02
#define COUNTER_RANGE_16MHz				0x03
#define COUNTER_RANGE_32MHz				0x04
#define COUNTER_RANGE_64MHz				0x05
#define COUNTER_RANGE_128MHz			0x06
//#define COUNTER_RANGE_256MHz			0x07
/** \} */

/**
 * \brief Measures a frequency with a selectable precision
 *
 * Measures a frequency with specific accuracy. The measurement-method
 * (direct/indirect) is automatically chosen depending on the estimated
 * result. If the measured frequency deviates too much from the estimate
 * the result might be not as precise as specified -> redo measurement.
 * Choosing a lower precision will result in a significantly faster
 * measurement.
 *
 * IMPORTANT:
 * All frequencies referenced in this function are expressed in Hz
 * AFTER the hardware prescaler. E.i. they differ from the actual applied
 * frequency for all prescalers != 1.
 * E.g. this function will return 50000 if called while using a prescaler
 * of 8 ant applying a signal with 800kHz
 *
 * \param precision Precision in terms of digits
 * \param estimate Estimated result in Hz
 * \return measured frequency in Hz
 */
uint32_t cnt_MeasureFrequency(uint8_t precision, uint32_t estimate);

/**
 * \brief Retrieves a ballpark value for the applied frequency
 *
 * IMPORTANT:
 * The returned frequency is expressed in Hz BEFORE the hardware prescaler
 * (e.i. the actual applied frequency). Also, the execution time increases
 *  at lower frequencies as the function steps through different prescalers
 *  taking multiple measurement.
 *
 * \return measured frequency in Hz (rough value)
 */
uint32_t cnt_GetEstimate(void);

/**
 * \brief Returns the optimal prescaler for a given frequency
 *
 * Calculates the optimal prescaler to achieve a frequency at
 * the input capture pin as high as possible but not higher than
 * COUNTER_MAX_INPUTPIN_FREQ
 *
 * \param estimate Estimated frequency at frontendCOUNTER_MAX_INPUTPIN_FREQ
 * \return Optimal prescaler
 */
uint8_t cnt_GetOptimalPrescaler(uint32_t estimate);

/**
 * \brief Takes a frequency measurement in a given range
 *
 * \param range Measurement range (see \ref counter_ranges)
 * \return measured frequency in Hz
 */
uint32_t cnt_TakeMeasurement(uint8_t *range);

#endif
