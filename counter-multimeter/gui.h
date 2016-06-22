#ifndef GUI_H_
#define GUI_H_

#include <stdint.h>

/**
 * IDs of menu entries
 */
#define ID_VOLTMETER_MANUELL    10
#define ID_VOLTMETER_AUTO       11
#define ID_VOLTMETER_500V		12
#define ID_VOLTMETER_200V		13
#define ID_VOLTMETER_20V		14
#define ID_VOLTMETER_2V			15

#define ID_AMPEREMETER_MANUELL  20
#define ID_AMPEREMETER_AUTO     21
#define ID_AMPEREMETER_10A		22
#define ID_AMPEREMETER_200mA    23
#define ID_AMPEREMETER_20mA     24
#define ID_AMPEREMETER_2mA      25
#define ID_AMPEREMETER_200uA    26

#define ID_WIDERSTAND_MANUELL   30
#define ID_WIDERSTAND_AUTO      31
#define ID_WIDERSTAND_20M		32
#define ID_WIDERSTAND_2M		33
#define ID_WIDERSTAND_200K		34
#define ID_WIDERSTAND_20K		35
#define ID_WIDERSTAND_2K		36

#define ID_DURCHGANG            40

#define ID_FREQUENZ             50

/**
 * Initializes the GUI
 */
void gui_Init(void);

/**
 * Selects a function from the menu
 * @return ID of selected function
 */
uint8_t gui_SelectFunction(void);

/**
 * Displays s welcome screen
 */
void gui_Welcome(void);

#endif /* GUI_H_ */