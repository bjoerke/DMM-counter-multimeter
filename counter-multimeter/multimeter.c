#include "multimeter.h"

void ADC_Init(void) {
	ADCSRA = 0x00;							// ADC Control and Status Register A
//	ADMUX = (1 << REFS0);// | (1 << ADLAR);									// REFS0 (Bit6 in ADMUX) = ADC Voltage Reference Selection => REFS[1:0] = 01 => AVcc with external capacitor at AREF pin
						 // ADLAR (Bit5 in ADMUX) = ADC Left Adjust Result (in this case TRUE)
						 // [REMOVED] MUX0 (Bit0 in ADMUX) = Analog Channel and Gain Selection Bit0 => MUX[4:0] = 00000 => Single Ended Input = ADC0

//	ACSR  =  0x80;															// Analog Comparator Control and Status Register = 0000 1000 => ACIE = 1 Analog Comparator Interrupt Enable

//	ADCSRA |= (1 << ADATE);													// ADATE = ADC Auto Trigger Enable
//	ADCSRB |= (1 << ADTS2);													// ADTS2 = ADC Trigger Select Bit2 (to be enabled in ADCSRB) => ADTS[2:0] = 010 => External Interrupt Request 0
	ADCSRA |= (1 << ADEN) /*| ( 1 << ADIE)*/| (1 << ADPS2) | (1 << ADPS1)
			| (1 << ADPS0);		// ADEN = ADC Enable
	// ADIE = ADC Interrupt Enable
	// ADPS1 (Bit1 in ADCSRA) = ADC Prescaler Select Bit1
	// ADPS0 (Bit0 in ADCSRA) = ADC Prescaler Select Bit0 => ADPS[2:0] = 011
	// => Division Factor = 128 (between system clock frequency and the input clock to the ADC)
	ADCSRA |= (1 << ADSC);						// ADSC = ADC Start Conversion

	while (ADCSRA & (1 << ADSC))	// Waiting for completion of the conversion.
	{
	}
	(void) ADCW;// Reading ADCW (ADCL + ADCH) once so the result of the next conversion can be used
}

uint16_t ADC_get(uint8_t channel) {
	ADMUX &= ~0x1f;
	ADMUX |= channel;

	ADCSRA |= (1 << ADSC);						// ADSC = ADC Start Conversion

	while (ADCSRA & (1 << ADSC))	// Waiting for completion of the conversion.
	{
	}
	return ADCW ;
}
void DMM_SetURange(uint8_t uRange) {
	// Select range
	switch (uRange) {
	case DMM_RANGE_2V:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_20V:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_200V:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_500V:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	}
	// Select voltage measurement
	shift_Clear(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetIRange(uint8_t iRange) {
	// Select range
	switch (iRange) {
	case DMM_RANGE_200uA:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_2mA:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Set(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_20mA:
		shift_Set(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_200mA:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	case DMM_RANGE_10A:
		shift_Clear(DMM_RANGESEL_LSB);
		shift_Clear(DMM_RANGESEL_MSB);
		break;
	}
	// Select Current measurement
	shift_Clear(DMM_R_SELECTOR);
	shift_Set(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetRRange(uint8_t rRange) {
	// Select range
	shift_Set(DMM_R_RANGE1);
	shift_Set(DMM_R_RANGE2);
	shift_Set(DMM_R_RANGE3);
	shift_Set(DMM_R_RANGE4);
	shift_Set(DMM_R_RANGE5);
	switch (rRange) {
	case DMM_RANGE_2kOhm:
		shift_Clear(DMM_R_RANGE1);
		break;
	case DMM_RANGE_20kOhm:
		shift_Clear(DMM_R_RANGE2);
		break;
	case DMM_RANGE_200kOhm:
		shift_Clear(DMM_R_RANGE3);
		break;
	case DMM_RANGE_2MOhm:
		shift_Clear(DMM_R_RANGE4);
		break;
	case DMM_RANGE_20MOhm:
		shift_Clear(DMM_R_RANGE5);
		break;
	}
	// Select Resistance measurement
	shift_Set(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

void DMM_SetCRange() {
	// Set mode
	shift_Set(DMM_RANGESEL_LSB);
	shift_Set(DMM_RANGESEL_MSB);

	// Select Resistance measurement
	shift_Set(DMM_R_SELECTOR);
	shift_Clear(DMM_U_I_SELECTOR);
	// update shift register content
	shift_Update();
}

// This function evaluates which parameter and which range is requested by the user
int32_t meter_measure(uint8_t variable, uint8_t range) {
	uint8_t i = 17;
	switch (variable) {
	case VOLTAGE:
		switch (range) {
		case A_RANGE: // TODO
			break;
		case M_RANGE1:
			DMM_SetURange(DMM_RANGE_500V);
			break;
		case M_RANGE2:
			DMM_SetURange(DMM_RANGE_200V);
			break;
		case M_RANGE3:
			DMM_SetURange(DMM_RANGE_20V);
			while (PINA & (1 << PINA5)) {
				// Wrote some stuff for testing purpose...
				LCD_WipeLine(2);
				LCD_GotoXY(0, 2);
				LCD_PutString("Test U: ");
				uint16_t adc = ADC_get(0);
				char buffer[10];
				itoa(adc, buffer, 10);
				LCD_PutString(buffer);
				LCD_Update();
				time_Waitms(100);
			}
			break;
		case M_RANGE4:
			DMM_SetURange(DMM_RANGE_2V);
			break;
		case M_RANGE5:						// does not exist in voltage-mode
			break;
		}
		break;
	case CURRENT:
		switch (range) {
		case A_RANGE:
		case M_RANGE1:
		case M_RANGE2:
		case M_RANGE3:
		case M_RANGE4:
		case M_RANGE5:
			break;
		}
		break;
	case RESISTANCE:
		switch (range) {
		case A_RANGE:
		case M_RANGE1:
		case M_RANGE2:
		case M_RANGE3:
		case M_RANGE4:
		case M_RANGE5:
			break;
		}
		break;
	case CONTINUITY:
		switch (range) {
		case A_RANGE:
		case M_RANGE1:
		case M_RANGE2:
		case M_RANGE3:
		case M_RANGE4:
		case M_RANGE5:
			break;
		}
		break;
	}

	return 1;
}
