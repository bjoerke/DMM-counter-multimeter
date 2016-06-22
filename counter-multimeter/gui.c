#include <string.h>
#include "gui.h"
#include "includes/lcd.h"
#include "joy.h"
#include "systime.h"

typedef struct
{
	const char* name;
	struct menu_t* next;
	uint8_t id;
}menu_entry_t;

typedef struct
{
	uint8_t num_entries;
	const char* title;
	uint8_t selected_entry;
	struct menu_t* parent;
	menu_entry_t entries[];
}menu_t;

menu_t menu_amp_manuell = {.num_entries = 5, .title = "Messbereich", .selected_entry = 0, .entries = {
	{ "10A", NULL, ID_AMPEREMETER_10A },
	{ "200mA", NULL, ID_AMPEREMETER_200mA },
	{ "20mA", NULL, ID_AMPEREMETER_20mA },
	{ "2mA", NULL, ID_AMPEREMETER_2mA },
	{ "200uA", NULL, ID_AMPEREMETER_200uA }
} };

menu_t menu_amp = {.num_entries = 2, .title = "Amperemeter", .selected_entry = 0, .entries = {
	{"Messmodus: Auto", NULL, ID_AMPEREMETER_AUTO},
	{"Messmodus: Manuell", &menu_amp_manuell, ID_AMPEREMETER_MANUELL}
} };

menu_t menu_volt_manuell = {.num_entries = 4, .title = "Messbereich", .selected_entry = 0,.entries = {
	{ "500V", NULL, ID_VOLTMETER_500V},
	{ "200V", NULL, ID_VOLTMETER_200V},
	{ "20V", NULL, ID_VOLTMETER_20V },
	{ "2V", NULL, ID_VOLTMETER_2V }
} };


menu_t menu_volt = {.num_entries = 2, .title = "Voltmeter",.selected_entry = 0, .entries = {
	{"Messmodus: Auto", NULL, ID_VOLTMETER_AUTO},
	{"Messmodus: Manuell", &menu_volt_manuell, ID_VOLTMETER_MANUELL}
} };

menu_t menu_res_manuell = {.num_entries = 5, .title = "Widerstand", .selected_entry = 0, .entries = {
	{ "20MOhm", NULL, ID_WIDERSTAND_20M },
	{ "2MOhm", NULL, ID_WIDERSTAND_2M },
	{ "200kOhm", NULL, ID_WIDERSTAND_200K },
	{ "20kOhm", NULL, ID_WIDERSTAND_20K },
	{ "2kOhm", NULL, ID_WIDERSTAND_2K }
} };

menu_t menu_res = {.num_entries = 2, .title = "Widerstand", .selected_entry = 0,.entries = {
	{"Messmodus: Auto", NULL, ID_VOLTMETER_AUTO},
	{"Messmodus: Manuell", &menu_res_manuell, 0}
} };

menu_t menu_main = {.num_entries = 5, .title = "Multimeter",.selected_entry = 0, .parent = &menu_main, .entries =  {
	{"Voltmeter", &menu_volt, 0  },
	{"Amperemeter", &menu_amp, 0 },
	{"Widerstand", &menu_res, 0 },
	{"Durchgangspruefung", NULL, ID_DURCHGANG },
	{"Frequenzmessung", NULL, ID_FREQUENZ }
} };


//renders a givven menu to LCD
static void display_menu(menu_t* menu)
{
	LCD_Clear();
	LCD_GotoXY(0,0);
	LCD_PutString(menu->title);
	LCD_PutChar('\n'); LCD_PutChar('\r');
	for(uint8_t i=0; i<strlen(menu->title); i++)
	{
		LCD_PutChar('-');
	}
	LCD_PutChar('\n'); LCD_PutChar('\r');
	for(uint8_t i=0; i<menu->num_entries; i++)
	{
		LCD_PutString("   ");
		LCD_PutString(menu->entries[i].name);
		LCD_PutChar('\n'); LCD_PutChar('\r');
	}
}

static menu_t* current_menu;

uint8_t gui_SelectFunction()
{
	display_menu(current_menu);
	LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar('>');
	LCD_Update();
	while(1) {
		uint8_t btn = joy_Wait();
		switch(btn)
		{
			case JOY_DOWN:
				LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar(' ');
				if(current_menu->selected_entry < current_menu->num_entries-1)
				{
					current_menu->selected_entry++;
				}
				else
				{
					current_menu->selected_entry = 0;
				}
				LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar('>');
				LCD_Update();
				break;
			case JOY_UP:
				LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar(' ');
				if(current_menu->selected_entry > 0)
				{
					current_menu->selected_entry--;
				}
				else
				{
					current_menu->selected_entry=current_menu->num_entries-1;
				}
				LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar('>');
				LCD_Update();
				break;
			case JOY_PUSH:
			case JOY_RIGHT:
			{
				menu_entry_t* entry = &current_menu->entries[current_menu->selected_entry];
				menu_t* next = entry->next;
				if(next == NULL)
				{
					LCD_Clear();
					LCD_GotoXY(0,0);
					LCD_PutString(entry->name);
					LCD_Update();
					return entry->id;
				}
				else
				{
					next->parent = current_menu;
					current_menu = next;
					display_menu(current_menu);
					LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar('>');
					LCD_Update();
				}
			}
			break;
			case JOY_LEFT:
				current_menu = current_menu->parent;
				display_menu(current_menu);
				LCD_GotoXY(0, current_menu->selected_entry+2); LCD_PutChar('>');
				LCD_Update();
				break;
		}
	}
}


void gui_Init(void)
{
	Backlight_LED(BL_BLUE_ON | BL_GREEN_ON | BL_RED_ON);
	current_menu = &menu_main;
}

void gui_Welcome(void)
{
		LCD_Clear();
		LCD_PutString_P(PSTR("	                   \r\n"));
		LCD_PutString_P(PSTR("	                   \r\n"));
		LCD_PutString_P(PSTR("    Willkommen zur   \r\n"));
		LCD_PutString_P(PSTR("     Demo-Version    \r\n"));
		LCD_PutString_P(PSTR("         des         \r\n"));
		LCD_PutString_P(PSTR("    Counter-Meters   \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_PutString_P(PSTR("                     \r\n"));
		LCD_Update();
		time_Waitms(500);
}