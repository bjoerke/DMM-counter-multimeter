#include "gui.h"

const char PROGMEM menuNames[GUI_NUM_MENU_ENTRIES][GUI_MAX_MENU_NAME_LENGTH + 1] =
		{ "VolDC", "VolAC", "AmpDC", "AmpAC", " Ohm ", "Beep ", " Hz  ",
				"Duty ", " CMD ", "Menu " };

// number of available ranges per menu entry
const uint8_t menuEntryRanges[GUI_NUM_MENU_ENTRIES] = { 5, 5, 6, 6, 6, 1, 7, 2,
		1, 1 };

// number of displayed digits for the different measurements
const uint8_t menuDisplayDigits[GUI_NUM_MEASUREMENT_ENTRIES] = { 5, 4, 4, 4, 5,
		4, 9, 3 };

// dot-position of the different measurements (0 = no dot)
const uint8_t menuDisplayDots[GUI_NUM_MEASUREMENT_ENTRIES] = { 2, 0, 1, 0, 0, 0,
		0, 1 };

// unit names of the different measurements
const char PROGMEM unitNames[GUI_NUM_MEASUREMENT_ENTRIES][6] = { "V(DC)",
		"V(AC)", "A(DC)", "A(AC)", "Ohm", "Ohm", "Hz", "%" };

const char PROGMEM rangeNames[GUI_NUM_MENU_ENTRIES][GUI_MAX_RANGES_PER_ENTRY][GUI_MAX_RANGE_NAME_LENGTH
		+ 1] = {
/* Range names for voltage DC */
{ " Autorange ", "  max: 2V  ", "  max:20V  ", " max: 200V ", " max: 500V " },
/* Range names for voltage AC */
{ " Autorange ", "  max: 2V  ", "  max:20V  ", " max: 200V ", " max: 500V " },
/* Range names for current DC */
{ " Autorange ", " max:200uA ", "  max:2mA  ", " max: 20mA ", " max:200mA ",
		"  max:10A  " },
/* Range names for current AC */
{ " Autorange ", " max:200uA ", "  max:2mA  ", " max: 20mA ", " max:200mA ",
		"  max:10A  " },
/* Range names for resistance */
{ " Autorange ", " max:2kOhm ", " max:20kOhm", "max:200kOhm", " max:2MOhm ",
		"max: 20MOhm" },
/* Range names for continuity */
{ "Continuity " },
/* Range names for frequency */
{ " Autorange ", "TTL (1MHz) ", " BNC 4MHz  ", " BNC 16MHz ", " BNC 32MHz ",
		" BNC 64MHz ", "BNC 128MHz " },
/* Range names for duty cycle */
{ "    TTL    ", "    BNC    " },
/* CMD 'range' names */
{ "UARTControl" },
/* Menu 'range' names */
{ "Enter Menu " } };

void gui_DisplayMainMenu(void) {
	// Display menu content
	LCD_Clear();
	// display selected entry + left and right entries
	uint8_t leftEntry =
			gui.selectedEntry > 0 ?
					gui.selectedEntry - 1 : GUI_NUM_MENU_ENTRIES - 1;
	uint8_t rightEntry = (gui.selectedEntry + 1) % GUI_NUM_MENU_ENTRIES;
	LCD_GotoXY(0, 7);
	LCD_PutString_P(menuNames[leftEntry]);
	LCD_GotoXY(8, 7);
	LCD_PutString_P(menuNames[gui.selectedEntry]);
	LCD_GotoXY(16, 7);
	LCD_PutString_P(menuNames[rightEntry]);
	// display arrow up
	LCD_GotoXY(10, 6);
	LCD_PutChar(0x5E);
	LCD_GotoXY(5, 5);
	LCD_PutString_P(
			rangeNames[gui.selectedEntry][gui.selectedRanges[gui.selectedEntry]]);
	LCD_GotoXY(0, 1);
	if (!gui.measurementActive) {
		// currently not measuring anything
		LCD_PutStringLarge("----------");
	} else {
		if (gui.measurementValid) {
			// display measurement result
			char result[12];
			gui_string_fromInt(gui.measurementResult, result,
					menuDisplayDigits[gui.selectedEntry],
					menuDisplayDots[gui.selectedEntry]);
			LCD_PutStringLarge(result);
		} else {
			// measurement out of range
			LCD_PutStringLarge("OutOfRange");
		}
		LCD_GotoXY(15, 3);
		LCD_PutString_P(unitNames[gui.selectedEntry]);
		// TODO the following is only hacked together and not nicely done!
		if (gui.selectedEntry <= GUI_MEASURE_RESISTANCE
				&& gui.selectedRanges[gui.selectedEntry] == 0) {
			// we are in an auto-ranging mode
			LCD_GotoXY(0, 0);
			LCD_PutString("Selected range:");
			LCD_PutChar(selectedAutoRange + '0');
		}
	}
	LCD_Update();
}

void gui_HandleUserInput(void) {
	if (joy_Pressed(JOY_LEFT)) {
		// switch to left entry
		// use some fancy animation, just because ;)
		uint8_t i;
		uint8_t leftEntry =
				gui.selectedEntry > 0 ?
						gui.selectedEntry - 1 : GUI_NUM_MENU_ENTRIES - 1;
		uint8_t rightEntry = (gui.selectedEntry + 1) % GUI_NUM_MENU_ENTRIES;
		uint8_t newLeftEntry =
				leftEntry > 0 ? leftEntry - 1 : GUI_NUM_MENU_ENTRIES - 1;
		for (i = 1; i < 8; i++) {
			LCD_WipeLine(7);
			// move leftEntry into middle and middle entry to the right
			LCD_GotoXY(0 + i, 7);
			LCD_PutString_P(menuNames[leftEntry]);
			LCD_GotoXY(8 + i, 7);
			LCD_PutString_P(menuNames[gui.selectedEntry]);
			// fade out right entry
			if (16 + i < 20) {
				LCD_GotoXY(16 + i, 7);
				LCD_PutString_P(menuNames[rightEntry]);
				// clear last column
				LCD_PutChar(' ');
			}
			// fade in new left entry
			if (i > 3) {
				LCD_GotoXY(0, 7);
				LCD_PutString_P(&menuNames[newLeftEntry][8 - i]);
			}
			LCD_Update();
			time_Waitms(50);
		}
		gui.selectedEntry = leftEntry;
		gui.measurementActive = 0;
	}
	if (joy_Pressed(JOY_RIGHT)) {
		// switch to right entry
		// use some fancy animation, just because ;)
		uint8_t i;
		uint8_t leftEntry =
				gui.selectedEntry > 0 ?
						gui.selectedEntry - 1 : GUI_NUM_MENU_ENTRIES - 1;
		uint8_t rightEntry = (gui.selectedEntry + 1) % GUI_NUM_MENU_ENTRIES;
		uint8_t newRightEntry = (gui.selectedEntry + 2) % GUI_NUM_MENU_ENTRIES;
		for (i = 1; i < 8; i++) {
			LCD_WipeLine(7);
			// move rightEntry into middle and middle entry to the left
			LCD_GotoXY(8 - i, 7);
			LCD_PutString_P(menuNames[gui.selectedEntry]);
			LCD_GotoXY(16 - i, 7);
			LCD_PutString_P(menuNames[rightEntry]);
			// fade out left entry
			if (i < 5) {
				LCD_GotoXY(0, 7);
				LCD_PutString_P(&menuNames[leftEntry][i]);
			}
			// fade in new right entry
			if (24 - i < 20) {
				LCD_GotoXY(24 - i, 7);
				LCD_PutString_P(menuNames[newRightEntry]);
				// clear last column
				LCD_PutChar(' ');
			}
			LCD_Update();
			time_Waitms(50);
		}
		gui.selectedEntry = rightEntry;
		gui.measurementActive = 0;
	}
	if (joy_Pressed(JOY_UP)) {
		// switch through ranges
		gui.selectedRanges[gui.selectedEntry] =
				(gui.selectedRanges[gui.selectedEntry] + 1)
						% menuEntryRanges[gui.selectedEntry];
		gui.measurementActive = 0;
	}
	if (joy_Pressed(JOY_PUSH)) {
		// enable measurement/enter menu
		if (gui.selectedEntry < GUI_NUM_MEASUREMENT_ENTRIES) {
			// selected entry is a measurement entry
			gui.measurementActive = 1;
		}
		if (gui.selectedEntry == GUI_ENTER_MENU) {
			gui_SettingsMenu();
		}
	}
}

void gui_TakeMeasurement(void) {
	switch (gui.selectedEntry) {
	case GUI_MEASURE_FREQUENCY:
		// convert auto range to 0xFF and take measurement
		gui.measurementResult = cnt_TakeMeasurement(
				gui.selectedRanges[GUI_MEASURE_FREQUENCY] > 0 ?
						gui.selectedRanges[GUI_MEASURE_FREQUENCY] :
						COUNTER_RANGE_AUTO);
		break;
	case GUI_MEASURE_DUTY:
		if (gui.selectedRanges[GUI_MEASURE_DUTY] == 0) {
			// measure duty cycle of TTL input
			counter_SelectInput(CNT_IN_TTL, CNT_TTL_PRE_1);
		} else {
			// measure duty cycle of BNC input
			counter_SelectInput(CNT_IN_LF, CNT_LF_PRE_1);
		}
		gui.measurementResult = counter_MeasureDuty(2000);
		break;
	case GUI_MEASURE_VOLTAGE_DC:
	case GUI_MEASURE_VOLTAGE_AC:
	case GUI_MEASURE_CURRENT_DC:
	case GUI_MEASURE_CURRENT_AC:
	case GUI_MEASURE_RESISTANCE:
	case GUI_MEASURE_CONTINUITY:
		gui.measurementValid = !meter_TakeMeasurement(&gui.measurementResult,
				gui.selectedEntry,
				gui.selectedRanges[gui.selectedEntry] > 0 ?
						(gui.selectedRanges[gui.selectedEntry]) :
						DMM_RANGE_AUTO);
		break;
	default:
		gui.measurementResult = 0;
		break;
	}
}

void gui_SettingsMenu(void) {
	uint8_t selectedEntry = 0;
	do {
		// display settings menu
		LCD_Clear();
		LCD_GotoXY(1, 0);
		LCD_PutString_PLarge(PSTR(" Settings"));
		LCD_GotoXY(1, 2);
		LCD_PutString_P(PSTR("Cnt-Ref:"));
		if (counter.refInternal) {
			LCD_PutString_P(PSTR("Internal"));
		} else if (counter.refExternal) {
			LCD_PutString_P(PSTR("External"));
		}
		LCD_GotoXY(0, 2 + selectedEntry);
		// display arrow
		LCD_PutChar(0x10);
		LCD_Update();
		time_Waitms(100);
		if (joy_Pressed(JOY_UP)) {
			// move arrow up (with wrap-around)
			selectedEntry =
					selectedEntry > 0 ?
							selectedEntry - 1 : GUI_NUM_SETTINGS_ENTRIES - 1;
		}
		if (joy_Pressed(JOY_DOWN)) {
			// move arrow down (with wrap-around)
			selectedEntry = (selectedEntry + 1) % GUI_NUM_SETTINGS_ENTRIES;
		}
		if (joy_Pressed(JOY_RIGHT | JOY_PUSH)) {
			// execute menu entry
			switch (selectedEntry) {
			case 0:
				// toggle counter reference
				if (!counter.refInternal) {
					counter_RefInternal();
				} else {
					if (counter_RefExternal()) {
						LCD_Clear();
						LCD_GotoXY(5, 0);
						LCD_PutString_PLarge(PSTR("Error"));
						LCD_GotoXY(0, 2);
						LCD_PutString_P(PSTR("Couldn't switch to\r\n"
								"external reference:\r\n"
								"Verify that 10MHz are\r\n"
								"applied at Ref BNC"));
						LCD_Update();
						while (!joy_Pressed(
								JOY_LEFT | JOY_RIGHT | JOY_UP | JOY_DOWN
										| JOY_PUSH))
							;
					}
				}
				break;
			}
		}
	} while (!joy_Pressed(JOY_LEFT));
}

void gui_string_fromInt(int32_t value, char *dest, uint8_t digits, uint8_t dot) {
	uint32_t divider = 1;
	uint8_t i;
	if (value < 0) {
		*dest++ = '-';
		value = -value;
	}
	for (i = 1; i < digits; i++)
		divider *= 10;

	for (i = digits; i > 0; i--) {
		if (i == dot)
			*dest++ = '.';
		uint32_t c = value / divider;
		*dest++ = (c % 10) + '0';
		value -= c * divider;
		divider /= 10;
	}
	*dest = 0;
}
