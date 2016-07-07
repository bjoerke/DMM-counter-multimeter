#ifndef _GUI_H_
#define _GUI_H_

#include <stdint.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "includes/lcd.h"
#include "joy.h"
#include "systime.h"
#include "counter.h"
#include "multimeter.h"
#include "uartProtocol.h"

#define GUI_MEASURE_VOLTAGE_DC			0
#define GUI_MEASURE_VOLTAGE_AC			1
#define GUI_MEASURE_CURRENT_DC			2
#define GUI_MEASURE_CURRENT_AC			3
#define GUI_MEASURE_RESISTANCE			4
#define GUI_MEASURE_CONTINUITY			5
#define GUI_MEASURE_FREQUENCY			6
#define GUI_MEASURE_DUTY				7
#define GUI_UART_PROTOCOL				8
#define GUI_ENTER_MENU					9

/* maximum length of one menu entry. Shorter entries should be centered */
#define GUI_MAX_MENU_NAME_LENGTH		5
/* Number of used menu entries */
#define GUI_NUM_MENU_ENTRIES			10
/* Number of measurement menu entries (the first X entries) */
#define GUI_NUM_MEASUREMENT_ENTRIES		8

/* Maximum available ranges/selections per menu entry */
#define GUI_MAX_RANGES_PER_ENTRY		7
/* maximum length of range/selection name. Shorter names should be centered */
#define GUI_MAX_RANGE_NAME_LENGTH		11

#define GUI_NUM_SETTINGS_ENTRIES		1

struct {
	/* currently selected entry */
	uint8_t selectedEntry;
	/* selected range for all menu entries */
	uint8_t selectedRanges[GUI_NUM_MENU_ENTRIES];
	/* currently taking measurements? */
	uint8_t measurementActive;
	/* result of the current measurement. Only valid if measurementActive = 1 */
	int32_t measurementResult;
	uint8_t measurementValid;
	char measurementUnit[6];
} gui;

/**
 * \brief Displays the main menu
 *
 * The main menu content is transferred to the LCD
 */
void gui_DisplayMainMenu(void);

/**
 * \brief Handles user input while in main menu
 */
void gui_HandleUserInput(void);

/**
 * \brief Samples a new measurement
 */
void gui_TakeMeasurement(void);

void gui_SettingsMenu(void);

void gui_UartProtocol(void);

/**
 * \brief Creates a string from an unsigned integer
 *
 * The string be will be created with leading zeros
 * and an optional fixed decimal point
 * \param value     Integer value which will be written to string
 * \param dest      Pointer to (big enough!) char array
 * \param digits    Number of displayed digits (not counting the decimal point).
 *                  If value has more digits than specified only the last
 *                  digits will be displayed. If value has less digits the
 *                  string will be filled with leading zeros
 * \param dot       Number of digits behind decimal point. If 0 the decimal point
 *                  will be omitted
 */
void gui_string_fromInt(int32_t value, char *dest, uint8_t digits, uint8_t dot);

#endif
